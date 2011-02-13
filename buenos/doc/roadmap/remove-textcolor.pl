#!/usr/bin/env perl

while($l = <STDIN>) {
    if ($l =~ /\/TeXcolorrgb\{setrgbcolor\}/) {
	$l =~ s/\/TeXcolorrgb\{setrgbcolor\}/\/TeXcolorrgb\{pop pop pop 0 0 0 setrgbcolor\}/;
    } elsif ($l =~ /\/TeXcolorrgb\{\s*$/) {
	print($l);
	$l = <STDIN>;
	$l =~ s/^\s*setrgbcolor\}/pop pop pop 0 0 0 setrgbcolor\}/;
    }
    print($l);
}
