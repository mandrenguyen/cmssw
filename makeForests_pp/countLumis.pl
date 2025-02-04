#!/bin/perl


@runs = `more runList.txt`;

foreach $run (@runs){

    chomp($run);
    
    @lines1 = `more aodRunLumi/run_$run.list`;
    
#$size = @lines; 
#print "size of array: $size. \n";
    
    $nlumis1=0;
    
    foreach $line (@lines1){
	
	chomp($line);
	#print $line."\n";
	
	if(grep(/^Fail/, $line) eq 0){	
	    #print $line."\n";
	    @commas = split(/,/, $line);
	    $ncommas = @commas;
	    #print $ncommas."\n";
	    $nlumis1= $nlumis1 +$ncommas;
	} 
    }
    
    
    @lines2 = `more rawRunLumi/run_$run.list`;
    
#$size = @lines; 
#print "size of array: $size. \n";
    
    $nlumis2=0;
    
    foreach $line (@lines2){
	
	chomp($line);
	#print $line."\n";
	
	if(grep(/^Fail/, $line) eq 0){	
	    #print $line."\n";
	    @commas = split(/,/, $line);
	    $ncommas = @commas;
	    #print $ncommas."\n";
	    $nlumis2= $nlumis2 +$ncommas;
	} 
    }
    

    #if($nlumis1 != $nlumis2){
	print $run.": \n";
	print "AOD: ".$nlumis1."\n";
	print "RAW: ".$nlumis2."\n";
    #}
}
