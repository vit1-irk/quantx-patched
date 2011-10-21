#!/usr/bin/perl

my $VERSION = shift;
die "Usage: $0 version\n" unless $VERSION;

my $v = $VERSION;
$v =~ s/\D//g;
my ($v4,$v3,$v2,@v1) = reverse split(//,$v);
$v3 ||= '0';
$v2 ||= '0';
my $v1 = join('',reverse @v1) || '0';
my $xoxxx = "$v1.$v2$v3$v4";
my $xoxox = "$v1.$v2.$v3.$v4"; 

my $disclaimer = "DO NOT EDIT! Generated with: perl $0 $VERSION";
open F, ">Version.nsh" or die;
print F <<EOH ;
\# $disclaimer
!define VERSION  "$xoxxx"
!define VERSIONA "$xoxox"
EOH
close F;

open R, "<Version.h" or die;
open W, ">Version.h.tmp" or die;
while (<R>)
{
    s/(KVANT_VERSION_STRING).*/$1 \"$xoxxx\"/;
    print W;
} 
close R;
close W;
rename("Version.h.tmp","Version.h");

open R, "<kvant.pro" or die;
open W, ">kvant.pro.tmp" or die;
while (<R>)
{
    s/(^VERSION =).*/$1 -$xoxxx/;
    print W;
} 
close R;
close W;
rename("kvant.pro.tmp","kvant.pro");

