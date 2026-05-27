use strict;
use warnings;
my ($file) = @ARGV;
open(my $fh, '<', $file) or die "open: $!";
my @lines = <$fh>;
close $fh;

my @out;
my $i = 0;
my $promoted_l1 = 0;
my $promoted_l2 = 0;
while ($i < @lines) {
    my $line = $lines[$i];

    # L1: triplet --\n--\tNAME\n--
    if ($line =~ /^--\s*$/ && $i + 2 < @lines) {
        my $mid = $lines[$i + 1];
        my $bot = $lines[$i + 2];
        if ($bot =~ /^--\s*$/) {
            if ($mid =~ /^(--\s+)([A-Z][A-Z0-9 _\/+\-]*[A-Z0-9])\s*\n?$/) {
                my ($pre, $name) = ($1, $2);
                # Avoid double-promote
                if ($name !~ /^#/) {
                    push @out, $line;
                    push @out, "${pre}# $name\n";
                    push @out, $bot;
                    $i += 3;
                    $promoted_l1++;
                    next;
                }
            }
        }
    }

    # L2: single-line uppercase content (not already promoted)
    if ($line =~ /^(--\s+)([A-Z][A-Z0-9 _\/+\-]*[A-Z0-9])\s*\n?$/) {
        my ($pre, $name) = ($1, $2);
        # skip if already has # prefix
        if ($name !~ /^#/) {
            push @out, "--## $name\n";
            $i++;
            $promoted_l2++;
            next;
        }
    }

    push @out, $line;
    $i++;
}

open(my $oh, '>', $file) or die "write: $!";
print $oh @out;
close $oh;

print STDERR "Promoted: $promoted_l1 level-1 (triplet) + $promoted_l2 level-2 (single-line)\n";
