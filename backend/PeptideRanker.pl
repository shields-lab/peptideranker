#!/usr/bin/perl
# CM Monday 13 December 2010 

#takes a file of peptides (peptide sequence on one line, no line break in sequence)
$pipeline_dir=".";

if(@ARGV < 2){
	print "Usage: ./PeptideRanker.pl input_file output_file\n";
	exit(0);
}

$fasta = shift @ARGV;
$out_file = shift @ARGV;

open(fin,"<$fasta");
@fasta = <fin>;
close fin;  

while(@fasta){
	$sequence = shift @fasta;
	$sequence =~ s/\s+$//; 
	$length = length($sequence);

	open (ft, ">peptide.dataset");
	print ft "1\npeptide\n$length\n$sequence\n1\n";
	close ft;

	if($sequence ne ""){
		if ($length <= 20) {
			system("$pipeline_dir/PeptideRanker $pipeline_dir/Models_Short.txt peptide.dataset ./ ");
		}
		else {
		    system("$pipeline_dir/PeptideRanker $pipeline_dir/Models_Long.txt peptide.dataset ./ ");
		}

		open(fin,"<peptide.dataset.predictions");
		@pred = <fin>;
		close fin;  

		shift @pred;
		shift @pred;
		shift @pred;
		shift @pred;
		shift @pred;
		$prediction = shift @pred;
		chop $prediction;
		@foo = split(/ /, $prediction);
		$prediction = $foo[1];

		push(@all,  "$prediction\t$sequence");
	}
	

}

@all = reverse sort(@all);

open(f,">$out_file");

for($i=0;$i<@all;$i++){
	print f "$all[$i]\n";
}

close f;

system("rm peptide.dataset peptide.dataset.predictions");




