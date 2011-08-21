#!/usr/bin/perl -w

sub normal
{
    local (*F,*T);
    my ($t,$f) = ("tmp.x",@_);

    open F, "$f" or die "Cannot open $f";
    open T, ">$t" or die "Cannot open >$t";
    while (<F>)
    {
        s/\s*\Z//;
        print T "$_\n"; 
    }
    close F;
    close T;
    if ( system("diff $f $t >/dev/null") )
    {
        print "$f updated\n";
        rename($t,$f);
    }
}

foreach my $file_cxx ( (glob("*.cpp"),glob("*.h")) )
{
    normal($file_cxx);
}
