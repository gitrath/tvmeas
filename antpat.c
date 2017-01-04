#include <assert.h>
#include <fstream.h>
#include <strstream.h>
#include <iomanip.h>
#include <string.h>
#include "antpat.h"

#define BAD_BEAMWIDTH       360.0

#define ANTENNANAME_LABEL   "antenna"
#define DATE_LABEL          "date"
#define FREQ_LABEL          "frequency"
#define PLANE_LABEL         "plane"
#define POLARIZATION_LABEL  "polarization"
#define DETECTMODE_LABEL    "detect_mode"
#define COMMENT_LABEL       "comment"


AntennaPattern::AntennaPattern() : angleVec(0),
                                   amplVec(0)
{
// cout << "AntennaPattern() called" << endl;
}

AntennaPattern::AntennaPattern(const char *fileName) : angleVec(0),
                                                 amplVec(0)
{
  read( fileName );
//  cout << "AntennaPattern(char *fileName) called" << endl;
}


AntennaPattern::AntennaPattern( const AntennaPattern &pat) :
     angleVec( pat.angleVec ), amplVec( pat.amplVec )
{
  for(int i=0; i< getNumLabels(); i++)
    {
     label[i] = pat.label[i];
    }
// cout << "AntennaPattern( AntennaPattern &pat ) called" << endl;

}

AntennaPattern::~AntennaPattern()
{
 // cout << "~AntennaPattern destructor called" << endl;
}


int AntennaPattern::size() const
{
  assert( angleVec.size() == amplVec.size() );
  return angleVec.size();
}

double AntennaPattern::getAngle(int index) const
{
  assert( index >= 0);
  assert( index < size() );

  return angleVec[index];
}

double AntennaPattern::getAmpl (int index) const
{
  assert( index >= 0);
  assert( index < size() );

  return amplVec[index];
}

double AntennaPattern::getMaxAmpl() const
{
  return amplVec.maxValue();
}

const char* AntennaPattern::getLabelName(LabelIndex i ) const
{
  assert ( i >= 0 );
  assert ( i <  getNumLabels() );


  switch (i)
    {
     case FILE_NAME:
       return "Filename";
     case ANTENNA:
       return "Antenna Name";
     case DATE:
       return "Date";
     case FREQUENCY:
       return "Frequency";
     case PLANE:
       return "Plane";
     case POLARIZATION:
       return "Polarization";
     case DETECT_MODE:
       return "Detect Mode";
     case BEAMWIDTH:
       return "Beamwidth";
     case SLL:
       return "Side Lobe Level";
     case COMMENT:
       return "Comment";
     default:
       assert(0);
    }
  return "ERROR";
}

const char *AntennaPattern::getLabel( LabelIndex i ) const
{
  assert( i >= 0 && i < getNumLabels() );
  if ( label[i] )
    {
     return label[i];
    }

  static char *nullString = "";
  return nullString;
}

void AntennaPattern::setLabel( LabelIndex i, const char * str )
{
  assert( i >= 0 && i < getNumLabels() );
  assert( str);

  label[i] = str;
}


AntennaPattern::LabelIndex AntennaPattern::findLabel(const char *labelName ) const
{
  assert( labelName );
  for( int i=0; i< getNumLabels(); i++)
    {
      if ( !strcmp( labelName, getLabelName(LabelIndex(i)) ))
        return LabelIndex(i);
    }
  return UNKNOWN_LABEL;
}


int AntennaPattern::getNumLabels() const
{
  return NUM_ANTENNA_PATTERN_LABELS;
}


void AntennaPattern::write( ostream &stream ) const
{
  assert( stream.good() );

  int i;

  for( i=0;i< getNumLabels(); i++)  // write labels to file
    {
     if ( i == FILE_NAME ) continue; // don't write file name to file
     if ( label[i].size() > 0 )
       {
        LabelIndex index = LabelIndex(i);
        stream << '#' << getLabelName(index) << ": " << getLabel(index) << endl;
       }
    }

  stream.setf( ios::showpoint );

  for(i=0;i<size();i++)  // write angle & amplitude points to file
    {
      stream << setprecision(2) << setw( 7 ) << angleVec[i] << ' ';
      stream << setprecision(2) << setw( 7 ) << amplVec[i] << endl;  // setf
    }

  assert(stream.good());
}

void AntennaPattern::write( const char *fileName ) const
{
  assert( fileName );

  ofstream stream( fileName, ios::out);
  assert( stream.good());

  write( stream );
}


void AntennaPattern::read( istream &stream)
{
  assert( stream.good() );

  clearLabels();  // erases label[] array (except FILENAME)

  int count = 0;

  while ( !stream.eof() )
    {

     const int MAXLINE=160;

     //while( stream.peek() == '\r' || stream.peek() == '\n' )
     //  stream.ignore(1);

     //if (stream.eof() ) break;

     char lineStr[ MAXLINE ];
     stream.getline(lineStr, MAXLINE);

     //assert( stream.eof() || stream.good() );
     if ( stream.eof() ) break; // CAUTION: must have a return after last line
     assert( stream.good() );

     if ( strlen(lineStr) == 0 ) continue; //skip blank lines

     istrstream line( lineStr, MAXLINE );

     if ( line.peek() == '#' )  // check for '#' at beginning of line
       {
        line.ignore(1);

        char labelName[ MAXLINE ];
        line.getline( labelName, MAXLINE, ':' );

        if ( line.peek() == ' ') line.ignore(1); // skip space after ':'

        char labelStr[ MAXLINE ];
        line.getline( labelStr, MAXLINE );

        if ( strlen( labelStr ) > 0 ) // check if a label was found
          {
           LabelIndex index = findLabel( labelName );
           if ( index != UNKNOWN_LABEL )
             {
              setLabel(index, labelStr);
             }
          }
       }
     else
       {
         double angle, ampl;

         line >> angle; assert( stream.good() );
         line >> ampl;  assert( stream.good() );

         //cout << angle << ' ' << ampl << endl;
         angleVec.force( count,angle);
         amplVec.force ( count, ampl);
         count ++;
       }
    }
  assert( count > 0 );

  angleVec.resize(count);
  amplVec.resize (count);
}



void AntennaPattern::read( const char *fileName)
{
  assert(fileName);

  ifstream stream( fileName, ios::in);
  assert( stream.good());


  // set FILE_NAME label
  const char *name = fileName;

  while( *name != '\0' ) name++;
  while( name>fileName && *(name-1) != '\\' && *(name-1) != '/' && *(name-1) != ':' ) name--;


  setLabel( FILE_NAME, name );
  read( stream );
}


static double interpolate( double x, double x1, double y1, double x2, double y2)
{
  return (y1-y2)/(x1-x2)*x + (x1*y2-x2*y1)/(x1-x2);
}


double AntennaPattern::beamwidth( double *left_angle, double *right_angle ) const
{
  return beamwidth( 3, left_angle, right_angle );
}


double AntennaPattern::beamwidth( double level_dB,
                  double *left_angle, double *right_angle ) const
{
  if (level_dB < 0 ) level_dB = -level_dB;

  assert( level_dB > 0);

  int    max_pos;
  double max_value = amplVec.maxValue( &max_pos );

  double right_dB_angle;

  for(int i=max_pos+1;;i++)
    {
      if( i >= size() ) return BAD_BEAMWIDTH;  // maybe should do something else
      assert(i<size());
      //assert( amplVec[i] <= amplVec[i-1] );

      if ( max_value-amplVec[i] == level_dB )
        {
         right_dB_angle = angleVec[i];
         break;
        }

      else if ( max_value-amplVec[i] > level_dB )
        {
         right_dB_angle = interpolate( level_dB, max_value-amplVec[i-1], angleVec[i-1],
					   max_value-amplVec[i],   angleVec[i] );
         break;
        }
    }

  double left_dB_angle;

  for(i=max_pos-1;;i--)
    {
      if( i< 0 ) return BAD_BEAMWIDTH;

      assert(i>=0);
      //assert( amplVec[i] <= amplVec[i+1] );

      if ( max_value-amplVec[i] == level_dB )
        {
         left_dB_angle = angleVec[i];
         break;
        }

      else if ( max_value-amplVec[i] > level_dB )
        {
         left_dB_angle = interpolate( level_dB, max_value-amplVec[i+1], angleVec[i+1],
					  max_value-amplVec[i],   angleVec[i] );
	 break;

        }
    }

  if (left_angle ) *left_angle  = left_dB_angle;
  if (right_angle) *right_angle = right_dB_angle;

  assert( right_dB_angle > left_dB_angle );

  return (right_dB_angle-left_dB_angle);
}

void AntennaPattern::clearLabels()  // doesn't clear filename
{
  assert( FILE_NAME == 0 );
  for(int i=FILE_NAME+1; i< NUM_ANTENNA_PATTERN_LABELS; i++)
    {
      label[i].setSize(0);
    }
}