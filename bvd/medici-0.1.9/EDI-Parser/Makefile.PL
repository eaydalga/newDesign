use ExtUtils::MakeMaker;
# See lib/ExtUtils/MakeMaker.pm for details of how to influence
# the contents of the Makefile that is written.

WriteMakefile(
	      'NAME'            => 'EDI::Parser',
	      'VERSION_FROM'    => 'Parser.pm', # finds $VERSION
	      'PREREQ_PM'       => {}, # e.g., Module::Name => 1.1
	      ($] >= 5.005 ?    ## Add these new keywords supported since 5.005
	       (ABSTRACT_FROM   => 'Parser.pm', # retrieve abstract from module
		'DEFINE'        => '-DHAS_CALL_METHOD -DHAS_SVPV_NOLEN',
		AUTHOR          => 'David Coles <david@disintegration.org>')
	       : ()),
	      'LIBS'            => ['-L../src -lmedici -lexpat'],
	      # Insert -I. if you add *.h files later:
	      'INC'		=> '-I../src',
	      # Un-comment this if you add C files to link with later:
	      'OBJECT'           => '$(O_FILES) ../examples/txttsg.o ../examples/xmltsg.o', # link all the C files too
);
