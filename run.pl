#!/usr/bin/perl
use strict;
use warnings;
use Config;
use Time::HiRes qw(time);
use IO::Pipe;

# ==========================================
# Configuration: Path to CSV file
# ==========================================
my $csv_file = "tasks.csv";

# ==========================================
# Configuration: Valid level values (0,1,2,3,4)
# ==========================================
my %valid_levels = (
    0 => 1,
    1 => 1,
    2 => 1,
    3 => 1,
    4 => 1
);

# ==========================================
# Validate input parameter
# ==========================================
my $target_level = @ARGV ? shift : 0;
unless ($target_level + 1) {
    die "Usage: $0 <level>\nValid levels: 0, 1, 2, 3, 4\nExample: $0 3\n";
}
unless ($valid_levels{$target_level}) {
    die "Invalid level: '$target_level'\nValid levels: 0, 1, 2, 3, 4\n";
}

# ==========================================
# Read and parse CSV file
# ==========================================
open my $fh, '<', $csv_file or die "[ERROR] Cannot open CSV file '$csv_file': $!\n";

my @tasks;
my $header_skipped = 0;

while (my $line = <$fh>) {
    chomp $line;
    next if $line =~ /^\s*$/;
    next if $line =~ /^\s*(#|\/\/|\/\*)/;

    unless ($header_skipped) {
        $header_skipped = 1;
        next;
    }

    my ($subdir, $cmd, $level) = split /,/, $line, 3;

    $subdir  =~ s/^\s+|\s+$//g;
    $cmd     =~ s/^\s+|\s+$//g;
    $level   =~ s/^\s+|\s+$//g;

    unless ($valid_levels{$level}) {
        die "[ERROR] Invalid level '$level' in CSV\nAllowed: 1,2,3,4\n";
    }

    push @tasks, [ "$subdir", $cmd, $level ];
}
close $fh;

unless (@tasks) {
    die "[ERROR] No valid tasks found in CSV\n";
}

# ==========================================
# Filter tasks by target level
# ==========================================
my @filtered = grep { $_->[2] == $target_level } @tasks;

unless (@filtered) {
    die "No tasks found for level $target_level\n";
}

print "Found " . scalar(@filtered) . " tasks for level $target_level\n";
print "Starting concurrent execution...\n\n";

# ==========================================
# Start overall timer
# ==========================================
my $overall_start = time();

# ==========================================
# Run tasks in parallel
# ==========================================
my @pipes;
my @child_pids;

for my $task (@filtered) {
    my ($subdir, $cmd, $level) = @$task;

    my $pipe = IO::Pipe->new();
    push @pipes, $pipe;

    my $pid = fork();

    if ($pid) {
        $pipe->reader();
        push @child_pids, $pid;
    }
    elsif (defined $pid) {
        $pipe->writer();
        $pipe->autoflush(1);

        print "[START] Subdir: $subdir | Cmd: $cmd\n";

        unless (chdir $subdir) {
            warn "[ERROR] Cannot enter dir $subdir: $!\n";
            print $pipe join("\t", $subdir, $cmd, "-1") . "\n";
            exit 1;
        }

        my $t0 = time();
        system($cmd);
        my $t1 = time();
        my $dur = $t1 - $t0;

        my $code = $? >> 8;
        my $signal = $? & 0x7F;
        my $core_dump  = $? & 0x80 ? 1 : 0;

        if ($signal != 0 || $core_dump != 0) {
          print "[FATAL] Command crashed! Signal: $signal | Core Dump: $core_dump | Exit Code: $code | Command: $cmd\n";
        }
        else {
          printf "[END] Subdir: $subdir | Exit: $code | Time: %.2fs\n", $dur;
        }

        print $pipe join("\t", $subdir, $cmd, $dur, $code, $signal, $core_dump) . "\n";
        exit $code;
    }
    else {
        die "[ERROR] Fork failed: $!\n";
    }
}

# ==========================================
# Collect timing results
# ==========================================
my @task_times;
for my $pipe (@pipes) {
    my $line = <$pipe>;
    chomp $line;
    my ($dir, $cmd, $drt, $code, $signal, $core_dump) = split /\t/, $line;
    push @task_times, { subdir => $dir, cmd => $cmd, duration => $drt, code => $code, signal => $signal, core_dump => $core_dump };
    close $pipe;
}

# ==========================================
# Wait all children
# ==========================================
waitpid $_, 0 for @child_pids;

# ==========================================
# Overall time
# ==========================================
my $total_time = sprintf("%.2f", time() - $overall_start);

# ==========================================
# Show top 15 slowest tasks
# ==========================================
my @sorted = sort { $b->{duration} <=> $a->{duration} } @task_times;
my $top = scalar(@sorted) < 15 ? scalar(@sorted) : 15;

# ==========================================
# Final output
# ==========================================
print "\n=========================================\n";
print "ALL TASKS(" . scalar(@filtered) . ") FINISHED\n";
print "Total time: $total_time seconds\n";
print "=========================================\n";

if ($top > 0) {
    print "\nTOP $top SLOWEST TASKS:\n";
    print "---------------------------------------------------------------------------------------------------------\n";
    printf "%-3s %-20s %-60s %-10s %-5s\n", "#", "Subdir", "Command", "Time(s)", "Exit status";
    print "---------------------------------------------------------------------------------------------------------\n";

    for my $i (0 .. $top - 1) {
        my $t = $sorted[$i];
        my $cmd_short = length($t->{cmd}) > 60 ? substr($t->{cmd}, 0, 37) . "..." : $t->{cmd};
				my $status_str;
				if ($t->{signal} != 0 || $t->{core_dump}) {
						$status_str = "SIG $t->{signal}";
						$status_str .= " core" if $t->{core_dump};
				} else {
						$status_str = "exit $t->{code}";
				}
        printf "%-3d %-20s %-60s %-10.2f %-10s\n", $i+1, $t->{subdir}, $cmd_short, $t->{duration}, $status_str;
    }
    print "---------------------------------------------------------------------------------------------------------\n";
}

# ==========================================
# Show tasks with non-zero exit code or crashed
# ==========================================
my @failed_tasks = grep { $_->{code} != 0 || $_->{signal} != 0 || $_->{core_dump} != 0} @task_times;

my $i = 0;
if (@failed_tasks > 0) {
    print "\nTASKS WITH NON-ZERO EXIT CODE:\n";
    print "---------------------------------------------------------------------------------------------------------\n";
    printf "%-7s %-15s %-60s %10s %10s\n", "Index", "Subdir", "Command", "Time(s)", "Exit status";
    print "---------------------------------------------------------------------------------------------------------\n";

    for my $t (@failed_tasks) {
        my $cmd_short = length($t->{cmd}) > 60 ? substr($t->{cmd}, 0, 37) . "..." : $t->{cmd};
				my $status_str;
				if ($t->{signal} != 0 || $t->{core_dump}) {
						$status_str = "SIG $t->{signal}";
						$status_str .= " core" if $t->{core_dump};
				} else {
						$status_str = "exit $t->{code}";
				}
        $i += 1;
        printf "%-7d %-15s %-60s %10.2f %10s\n", $i, $t->{subdir}, $cmd_short, $t->{duration}, $status_str;
    }
    print "---------------------------------------------------------------------------------------------------------\n";
}

system("git status")
