<%inherit file="/base.mako"/>


<%def name="stylesheets()">
    ${parent.stylesheets()}
    ${h.css('hanger')}
</%def>

<%def name="body()">

<div class="start_info">
	<div class="gauge_div">
	<dl>
	<dt>
	    <dd>
		View a <a target="_self" class="reference" href="/static/organisms.html">list of supported genomes</a> from 
		<a class="reference" href="http://www.eupathdb.org/">EuPathDB</a>, 
		<a class="reference" href="http://www.patricbrc.org/">PATRIC</a>, 
		and <a class="reference" href="http://www.vectorbase.org/">VectorBase</a>. 
	    </dd>
	    <br/>
	     <dd>
	     Have a question? <a href="mailto:pathporthelp@vbi.vt.edu" title="Send us email">Contact the Pathogen Portal Team</a>
	     </dd>
	</dt>
	</div>

	<div id="flowchart_area"></div>
</div>
<script type="text/javascript" src="${h.url_for('/static/scripts/d3.v2.min.js')}"></script>
<script type="text/javascript" src="${h.url_for('/static/scripts/flowchart_diagram.js')}"></script>
${h.js( "libs/jquery/jquery.blockUI")}
<script type="text/javascript">
    function stepSelected(id) {
        //first, unhighlight everything 
        d3.selectAll(".active_menu").classed("active_menu", false).classed("regular_menu", true);
        if(id != null && id.length >0) {
            //highlight appropriate menu items
            d3.selectAll("." + id).classed("regular_menu", false).classed("active_menu", true);
        }
    }
    $(document).ready(function () {
        $.blockUI.defaults.overlayCSS.opacity = .40;
	$.blockUI.defaults.overlayCSS.cursor = 'pointer';
	$('#center').attr('margin-left','0px');
	$("body").attr('margin','0px');
        var logged_in = false;
	%if trans.get_user():
            logged_in = true;
        %endif
        if ( ! logged_in) {
            $('#task_area').block({
                message: '<h1 style="color: rgb(214,220,223);"><a href="${h.url_for(controller='user', action='login', target='galaxy_main')}" style="color: rgb(214,220,223);">Login to get started</a></h1>',
                centerY: 0,
		css: { border: '2px solid transparent', top: '60px', cursor: 'pointer', backgroundColor: 'transparent', color: 'rgb(255,255,255)' },
            });
        }
    });
</script>

   <div id="task_area"class="centered">
    <span >
	<table class="hanger">
	<tbody><tr>
	<th class="hanger" colspan="1"></th>
	<th class="hanger" colspan="2">Choose an activity below</th>
	<th class="hanger" colspan="1"></th>
	</tr>
	<tr>
	    <td class="hanger_border" style="padding-top: 0em"> </td>
	    <td class="hanger_image" nowrap="" style="padding-bottom: 0em"> 
		<img src="${h.url_for('/static/images/Keyboard-12.png')}" width="65px" height="64px" alt="Upload">
	    </td>
	    <td class="hanger_main" style="padding-bottom: 0em; vertical-align:bottom;">
		<h1 class="hanger"> Uploads </h1>
	    </td>
	    <td class="hanger_border" style="padding-top: 0em"> </td>
	</tr>
	<tr>    
	    <td class="hanger_border" style="padding-top: 0em"> </td>
	    <td class="hanger_image" style="padding-top: 0em"> </td> 
	    <td class="hanger_main" style="padding-top: 0em">
	    <a href="${h.url_for(controller='launch_pad', action='leave_hanger', launch_type='upload', retrieval_tag='upload1')}"><b>Upload Files</b></a><br> Upload files for analysis via URL, FTP, or HTTP.<br>
	    <a href="${h.url_for(controller='launch_pad', action='leave_hanger', launch_type='upload', retrieval_tag='ebi_sra_main')}"><b>Get fastq files from SRA/ENA</b></a><br>
	    Search for SRA, ENA, GEO, or ArrayExpress accessions<br>
	    </td>        
	    <td class="hanger_border" style="padding-top: 0em"> </td>
	</tr>
	
	<tr>
	    
	    <td class="hanger_border" style="padding-top: 0em"> </td>
	    <td class="hanger_image" nowrap="" style="padding-bottom: 0em"> 
		<img src="${h.url_for('/static/images/Status-2.png')}" width="65px" height="64px" alt="QC">
	    </td>
	    <td class="hanger_main" style="padding-bottom: 0em; vertical-align:bottom;">
		<h1 class="hanger"> Quality Control </h1>
	    </td>
	    <td class="hanger_border" style="padding-top: 0em"> </td>
	</tr>
	<tr>    
	    <td class="hanger_border" style="padding-top: 0em"> </td>
	    <td class="hanger_image" style="padding-top: 0em"> </td>    
	    <td class="hanger_main" style="padding-top: 0em">
	    <div id="fastqc" class="regular_menu reads_quality_check">
	    <a href="${h.url_for(controller='launch_pad', action='leave_hanger', launch_type='workflow', retrieval_tag='fastqc')}"><b>Check read quality</b></a><br>
	    Optional: Run FastQC to get a report on the quality of base calls that could affect your read mapping.<br>
	    </div>
	    <div id="sickle" class="regular_menu trimming">
	    <a href="${h.url_for(controller='launch_pad', action='leave_hanger', launch_type='workflow', retrieval_tag='sickle')}"><b>Trim Reads</b></a><br>
	    Optional: Run the sickle trimming tool to trim your reads and prepare them for alignment.<br>
	    </div>
	    <div id="samstat" class="regular_menu mapping_quality_check">
	    <a href="${h.url_for(controller='launch_pad', action='leave_hanger', launch_type='workflow', retrieval_tag='samstat')}"><b>Check mapping quality</b></a><br> 
	    Optional: Check the number of reads mapped and the alignment quality.<br>
            </div>
	    </td>
	    <td class="hanger_border" style="padding-top: 0em"> </td> 
	</tr>
	  
	
	<tr>
	    <td class="hanger_border" style="padding-top: 0em"> </td>
	    <td class="hanger_image" nowrap="" style="padding-bottom: 0em">   
		 <img src="${h.url_for('/static/images/Health-6.png')}" width="65px" height="64px" alt="Analysis"> 
		  
	    </td>
	    <td class="hanger_main" style="padding-bottom: 0em; vertical-align:bottom;">
		<h1 class="hanger"> RNA-Seq Analysis </h1>
	    </td>
	</tr>
	<tr>    
	    <td class="hanger_border" style="padding-top: 0em"> </td>
	    <td class="hanger_image" style="padding-top: 0em"> </td>
	    
	    <td class="hanger_main" style="padding-top: 0em"> 
	    <div id="pipeline" class="regular_menu transcript_assembly mapping">
		<a href="${h.url_for(controller='launch_pad', action='leave_hanger', launch_type='workflow', retrieval_tag='pipeline')}"><b>Align Reads &amp; Assemble Transcripts</b></a><br> 
		Map your reads to the genome and assemble them into transcripts. The alignment step will generate BAM files and the assembly step will generate BED and GTF files.<br>
            </div>
	    <div id="cuffde" class="regular_menu differential_expression_analysis">
		<a href="${h.url_for(controller='launch_pad', action='leave_hanger', launch_type='tool', retrieval_tag='cuffde')}"><b>Differential Expression Analysis</b></a><br> 
		Test RNA-Seq samples to determine if transcripts are differentially expressed.<br>
            </div>
	    <div id="cuffdiff_to_genelist" class="regular_menu differential_expression_analysis">
		<a href="${h.url_for(controller='launch_pad', action='leave_hanger', launch_type='tool', retrieval_tag='cuffdiff_to_genelist')}"><b>Create GeneList Summary</b></a><br> 
		Create a GeneList file, for use in PATRIC and other differential expression analysis tools.<br>
            </div>
	    </td>
	    <td class="hanger_border" style="padding-top: 0em"> </td>
	</tr>    
	<tr>
	    <td class="hanger_border" style="padding-top: 0em"> </td>
	    <td class="hanger_image" nowrap="" style="padding-bottom: 0em">   
		 <img src="${h.url_for('/static/images/Interface-86.png')}" width="65px" height="64px" alt="Extra"> 
		  
	    </td>
	    <td class="hanger_main" style="padding-bottom: 0em; vertical-align:bottom;">
		<h1 class="hanger"> Additional Tools </h1>
	    </td>
	    <td class="hanger_border" style="padding-top: 0em"> </td>
	</tr>
	<tr>    
	    <td class="hanger_border" style="padding-top: 0em"> </td>
	    <td class="hanger_image" style="padding-top: 0em"> </td>
	    <td class="hanger_main" style="padding-top: 0em">      
	    <div id="bedtools" class="regular_menu">
		<a href="${h.url_for(controller='launch_pad', action='leave_hanger', launch_type='tool_list', retrieval_tag='bedtools', tool_list='Convert BAM to BED:bedtools_bamtobed,Create a BedGraph of Genome Coverage:bedtools_genomecoveragebed_bedgraph,Create a Histogram of Genome Coverage:bedtools_genomecoveragebed_histogram')}"><b>BED Tools</b></a><br> 
		Use BEDTools to create summary BED files for analysis of genome coverage.<br>
             </div>
	    <div id="rgPicardMarkDups" class="regular_menu deduplication">
		<a href="${h.url_for(controller='launch_pad', action='leave_hanger', launch_type='tool', retrieval_tag='rgPicardMarkDups')}"><b>Remove Duplicate Reads</b></a><br> 
		Optional: PCR amplification can lead to bias. For paired-end reads only: if multiple pairs of reads have the exact same cooridnates mark all except one as a duplicate and remove.<br>
            </div>
	    <div id="aligners" class="regular_menu mapping">
		<a href="${h.url_for(controller='launch_pad', action='leave_hanger', launch_type='tool_list', retrieval_tag='aligners', tool_list='Prokaryotic Reads:bowtie2,Eukaryotic Reads:tophat2')}"><b>Alignment Only</b></a><br>
		To use advanced alignment parameters and/or perform alignment against a non-BRC genome.<br>
            </div>
	    <div id="assemblers" class="regular_menu transcript_assembly">
		<a href="${h.url_for(controller='launch_pad', action='leave_hanger', launch_type='tool_list', retrieval_tag='assemblers', tool_list='Assembly using non-BRC annotation:cufflinks_non,Prokaryotic Assembly:cufflinks_prok,Eukaryotic Assembly:cufflinks_euk')}"><b>Transcript Assembly Only</b></a><br> 
		To use advanced assembly parameters and/or perform assembly using non-BRC annotations.<br>
            </div>
	    <div id="cuffcompare" class="regular_menu">
		<a href="${h.url_for(controller='launch_pad', action='leave_hanger', launch_type='tool', retrieval_tag='cuffcompare')}"><b>Compare Transcript Assembly to Existing annotation</b></a><br> 
		Compare a cufflinks based transcript assembly to an existing annotation.<br>
            </div>
	    <div id="peakcalling_macs14" class="regular_menu">
		<a href="${h.url_for(controller='launch_pad', action='leave_hanger', launch_type='workflow', retrieval_tag='peakcalling_macs14')}"><b>ChIP-Seq: Call Peaks </b></a><br>
		Call peaks by providing mapped reads to the MACS software.<br>
            </div>
	    <div id="solid2fastq" class="regular_menu">
		<a href="${h.url_for(controller='launch_pad', action='leave_hanger', launch_type='workflow', retrieval_tag='solid2fastq')}"><b>Convert to FastQ Format</b></a><br>
		Optional: Use this tool to convert SOLiD reads to FastQ Sanger format.<br>
            </div>
	    <div id="solid" class="regular_menu mapping">
		<a href="${h.url_for(controller='launch_pad', action='leave_hanger', launch_type='tool_list', retrieval_tag='solid', tool_list='Prokaryotic Reads:bowtie_color,Eukaryotic Reads:tophat_color')}"><b>Align SOLiD reads</b></a><br>
		Align SOLiD reads that use color space.<br>
            </div>
	    </td>
	    <td class="hanger_border" style="padding-top: 0em"> </td>
	</tr>    

    </tbody></table>
    </span></div>

<div id="bottom_info">
<hr/>
	        Galaxy is supported in part by <a target="_blank" class="reference" href="http://www.nsf.gov">NSF</a>, <a target="_blank" class="reference" href="http://www.genome.gov">NHGRI</a>, and <a target="_blank" class="reference" href="http://www.huck.psu.edu">the Huck Institutes of the Life Sciences</a><br /><br />
</div>


<div id="footer_container">
<table border="0" width="100%">
    <tr>
        <td>
            <a href="https://www.vbi.vt.edu/"><div id="vbi_logo"></div></a>
        </td>
        <td>
            <div id="funding_notice">
                This project has been funded in whole or in part with Federal funds
                from the National Institute of Allergy and Infectious Diseases, National
                Institutes of Health, Department of Health and Human Services,
                under Contract No. HHSN272200900040C, awarded to BWS Sobral.
            </div>
        </td>
        <td>
            <div id="contact_email">
                <a href="mailto:pathporthelp@vbi.vt.edu" title="Send us email">Contact the Pathogen Portal Team</a>
            </div>
        </td>

        <td>
            <div id="bottom_brc_links">
<a href="/portal/portal/PathPort/Home" title="Go to Pathogen Portal Homepage" alt="Link to Pathogen Portal Homepage">Pathogen Portal</a>
|
<a href="http://www.eupathdb.org" title="Go to EuPathDB Homepage "alt="Link to EuPathDB Homepage">EuPathDB</a>
|
<a href="http://www.fludb.org" title="Go to IRD Homepage" alt="Link to IRD Homepage">IRD</a>
|
<a href="http://www.patricbrc.org" title="Go to PATRIC Homepage" alt="Link to PATRIC Homepage">PATRIC</a>
|
<a href="http://www.vectorbase.org" title="Go to VectorBase Homepage" alt="Link to VectorBase Homepage">VectorBase</a>
|
<a href="http://www.viprbrc.org" title="Go to ViPR Homepage" alt="Link to ViPR Homepage">ViPR</a>
            </div>
        </td>

    </tr>
</table>

    </div>
</%def>
