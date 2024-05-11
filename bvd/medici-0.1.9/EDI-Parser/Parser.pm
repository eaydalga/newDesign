#! /usr/bin/perl
package EDI::Parser;

# Comment out the next three lines and remove the "our " at the
# beginning of declarations to get this to work with older Perl (like
# 5.00405 which shops with IRIX)
use 5.006;
use strict;
use warnings;

require Exporter;
require DynaLoader;

our @ISA = qw(Exporter DynaLoader EDI::Parser::MEDICI);


# Items to export into callers namespace by default. Note: do not export
# names by default without a very good reason. Use EXPORT_OK instead.
# Do not simply export all your public functions/methods/constants.

# This allows declaration	use EDI::Parser ':all';
# If you do not need this, moving things directly into @EXPORT or @EXPORT_OK
# will save memory.
our %EXPORT_TAGS = ( 'all' => [ qw( EDIFACT UNGTDI X12 ) ] );
our @EXPORT_OK = ( @{ $EXPORT_TAGS{'all'} });
our @EXPORT = qw( );

our $VERSION = '0.01';

bootstrap EDI::Parser $VERSION;
use strict;

# Preloaded methods go here.

# **********************************************************************

sub new {
    my($pkg, %args) = @_;

    my $parser = $pkg->SUPER::new();
    
    $parser->setHandlers(%{$args{Handlers}}) if defined $args{Handlers};

    if(defined $args{Style} && $args{Style} eq 'Subs') {
	$parser->setHandlers(Start   => \&handleStart,
			     End     => \&handleEnd,
			     Segment => \&handleSegment);
    }
    
    $parser->{_Pkg} = defined $args{Pkg} ? $args{Pkg} : caller;

    return $parser;
}

sub handleStart {
    my($parser, $event, %parameters) = @_;
    no strict 'refs';

    my $method = $parser->{_Pkg} . "::$event";
    eval { &$method($parser, $event, %parameters) };
}

sub handleEnd {
    my($parser, $event, %parameters) = @_;
    no strict 'refs';

    my $method = $parser->{_Pkg} . "::_$event";
    eval { &$method($parser, $event, %parameters) };
}

sub handleSegment {
    my($parser, $segment, %parameters) = @_;
    no strict 'refs';

    my $method  = $parser->{_Pkg} . '::__' . $segment->code;
    my $default = $parser->{_Pkg} . '::segment';

    eval { &$default($parser, $segment, %parameters) }
    unless eval { &$method($parser, $segment, %parameters) };
}









package EDI::Parser::MEDICI;
sub new {
    my($pkg, %args) = @_;
    my($key, $val);
    my $parser = &parser_create();
    my $obj = bless { _parser => $parser }, $pkg;
    &parser_user_data($parser, $obj);
    $obj->{'_'.$key} = $val while ($key, $val) = each(%args);
    return $obj;
}

sub DESTROY {
    my($self) = @_;
    &parser_free($self->{_parser}) if defined $self->{_parser};
}

sub error {
    my($self) = @_;
    return &parser_get_error_code($self->{_parser});
}

sub status {
    my($self) = @_;
    return &get_error_string($self->error);
}

sub abort {
    my($self) = @_;
    &parser_abort($self->{_parser});
}

sub type {
    my($self) = @_;
    return &parser_interchange_type($self->{_parser});
}

sub parse {
    my($self, $buffer, $done) = @_;
    $done = defined $done ? $done : 0;
    return &parser_parse($self->{_parser}, $buffer, $done ? 1 : 0);
}

sub readtsg {
    my($self, $file) = @_;
    return undef unless defined $file;
    return &parser_xmltsg_fd($self, fileno($file)) if ref($file) eq 'GLOB';
    return &parser_xmltsg($self, $file . "\0") if $file =~ /\.xml$/;
    return &parser_txttsg($self, $file . "\0");
}

sub setHandlers {
    my($self, %handlers) = @_;
    while(my($key, $val) = each %handlers) {
	$self->{_Handlers}->{$key} = $val;
    }
}

sub startHandler {
    my($self, $event, $parameters) = @_;
    my $handler = $self->{_Handlers}->{Start};
    return &{$handler}($self, $event, %$parameters) if defined $handler;
}

sub endHandler {
    my($self, $event, $parameters) = @_;
    my $handler = $self->{_Handlers}->{End};
    #return &{$handler}($self, $event, %$parameters) if defined $handler;
    return &{$handler}($self, $event) if defined $handler;
}

sub segmentHandler {
    my($self, $segment, $directory, $parameters) = @_;
    my($handler);
    my %parameters = %$parameters;
    
    $segment = EDI::Parser::Segment->new($segment, $directory);
    $handler = $self->{_Handlers}->{Segment};
    return &{$handler}($self, $segment, %$parameters) if defined $handler;
}

sub tokenHandler {
    my($self, $token) = @_;
    my $handler = $self->{_Handlers}->{Token};
    
    $token = EDI::Parser::Token->new($token);
    return &{$handler}($self, $token) if defined $handler;
}

sub errorHandler {
    my($self, $code) = @_;
    my $string = &get_error_string($code);
    my $handler = $self->{_Handlers}->{Error};
    return &{$handler}($self, $code, $string) if defined $handler;
}

sub warningHandler {
    my($self, $code) = @_;
    my $string = &get_error_string($code);
    my $handler = $self->{_Handlers}->{Warning};
    return &{$handler}($self, $code, $string) if defined $handler;
}

sub directoryHandler {
    my($self, $parameters) = @_;
    my $return;
    my $handler = $self->{_Handlers}->{Directory};
    $return = &{$handler}($self, %$parameters) if defined $handler;
    return defined $return ? $return : 0;
}



package EDI::Parser::Token;

sub new   { bless { _token => $_[1] }, $_[0] }
sub raw   { &token_get_rdata ($_[0]->{_token}) }
sub data  { &token_get_cdata ($_[0]->{_token}) }
sub type  { &token_get_type  ($_[0]->{_token}) }
sub last  { &token_is_last   ($_[0]->{_token}) }
sub first { &token_is_first  ($_[0]->{_token}) }


package EDI::Parser::Segment;
sub new { bless { _segment => $_[1], _directory => $_[2] }, $_[0] }

sub code { &segment_get_code ($_[0]->{_segment}) }
sub name { &directory_get_segment_name ($_[0]->{_directory}, $_[0]->code) }

sub elementCount    { &segment_element_count   ($_[0]->{_segment}) }
sub subelementCount { &segment_subelement_count($_[0]->{_segment}, $_[1]) }

sub element {
    my($self, @indexes) = @_;
    my($index, @elements, $element, $subelement, $ref, $val);

    foreach $index (@indexes) {
	$val = undef;
	$ref = ref($index);
	
	if($ref eq 'ARRAY') {
	    $val = &segment_get_element	($self->{_segment}, @{$index});
	} else {
	    ($element, $subelement) = split(/\//, $index);
	    $val = &segment_get_element_by_name
		($self->{_segment}, $self->{_directory},
		 "$element\0", "$subelement\0");
	}

	push(@elements, $val);
    }
    wantarray ? return @elements : $elements[0];
}











# **********************************************************************

1;
__END__
# Below is stub documentation for your module. You better edit it!

=head1 NAME

EDI::Parser - Perl extension for the MEDICI EDI library

=head1 SYNOPSIS

 use EDI;

 $p1 = new EDI::Parser(Handlers => {Start   => \&handle_start,
				    End     => \&handle_end,
				    Segment => \&handle_segment,
				    Warning => \&handle_warning,
				    Error   => \&handle_error});

 $p1->parse($buffer, $done);

 $status = $p1->status;



=head1 DESCRIPTION

This module provides a way to parse EDI transmissions. It is built on
top of the MEDICI Electronic Data Interchange library.

=head1 THE API INTERFACE


=over 5




=item B<$value = $segment-E<gt>element($elemspec [, $elemspec]) ;>

Retrieves the value of the element specified by
C<$elemspec>. C<$elemspec> can be a scalar of the form
"<element-name>" or "<composite-name>/<element-name>" (eg. "1082" or
"C273/7008") or alternately an arrary reference specifying the indexes
of the required element, such as [0,0] for the first sub-element of
the first composite element, or [5,3] for the fourth sub-element of
the sixth composite element. The arrary reference form can be used
when no transaction-set guidelines are available to map element names
to segment position.

=back



=head1 EXPORTS

None by default. EDIFACT, UNGTDI and X12 may be imported.

=head1 AUTHOR

David Coles E<lt>david@disintegration.orgE<gt>

=head1 SEE ALSO

L<perl(1)>.

=cut
