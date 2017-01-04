#include <time.h>
#include <math.h>
#include "plotpat.h"
#include "antpat.h"
#include "string.h"
#include "stdio.h"

#define  RADIUS        3.0    // polar plot radius in inches
#define  XCENT         3.5    // polar plot center x in inches
#define  YCENT         3.25   // polar plot center y in inches
#define  LEGEND_LEFT   7.0    // legend left side in inches
#define  LEGEND_RIGHT  9.75   // legend right side in inches
#define  LEGEND_TOP    6.5    // legend top in inches
#define  LEGEND_INSET  0.5    // space in legend to allow for colored horz lines
#define  TEXT_VSPACING 0.1875 // vertical spacing between lines of text

PlotPattern::PlotPattern( Plot *plot_driver ) :
           patternList( 10 ), title(),
           uncommonLabels( NUM_ANTENNA_PATTERN_LABELS ),
           commonLabels  ( NUM_ANTENNA_PATTERN_LABELS )
{
  plotDriver = plot_driver;

  setMinMax();

  setAngleGrid();
  setAmplGrid();

  setAngleLabels();
  setAmplLabels();

  plotTimeStr[0] = '\0';
  plotDateStr[0] = '\0';

  setupLegend();
}


void PlotPattern::setTitle( const char *title_string )
{
  assert( title_string );
  title = title_string;
}

void PlotPattern::setMinMax( double min_ampl_dB,
                             double max_ampl_dB)
{
  assert( max_ampl_dB > min_ampl_dB );

  minAmpl = min_ampl_dB;
  maxAmpl = max_ampl_dB;
}


void PlotPattern::addPattern( const AntennaPattern *pat )
{
  patternList.addItem( (AntennaPattern *) pat );
}



void PlotPattern::setAngleGrid( double spacing_deg )
{
  assert( spacing_deg>0);
  angleGridSpacing = spacing_deg;
}

void PlotPattern::setAmplGrid( double spacing_dB )
{
  assert( spacing_dB > 0);
  amplGridSpacing = spacing_dB;
}

void PlotPattern::setAngleLabels( double spacing_deg )
{
  assert( spacing_deg >= 0);  // zero for no angle labels
  angleLabelSpacing = spacing_deg;
}

void PlotPattern::setAmplLabels( double spacing_dB)
{
  assert( spacing_dB >= 0);  // zero for no ampl labels
  amplLabelSpacing = spacing_dB;
}


void PlotPattern::plot()
{
  setupLegend();
  plotAmplGrid();
  plotAngleGrid();
  plotAllPatterns();
  plotLegend();
  plotCommonInfoText();
  plotTitle();
  plotDate();
  plotAmplLabels();
  plotAngleLabels();
}

void PlotPattern::plotAmplGrid()
{
  plotDriver->setColor( Plot::GREEN );

  assert( amplGridSpacing > 0 );
  int numCircles = int ( (maxAmpl-minAmpl)/amplGridSpacing );

  double spacing = amplGridSpacing/(maxAmpl-minAmpl)*RADIUS;

  for(int i=0; i <= numCircles;  i++)
    {
     plotDriver->circle( XCENT, YCENT, RADIUS-spacing*i );
    }
}

void PlotPattern::plotAngleGrid()
{
  plotDriver->setColor( Plot::GREEN );

  assert( angleGridSpacing > 0);
  int numLines = int( 180.0/angleGridSpacing );

  for(int i=0;i<numLines;i++)
    {
      double angle = i*angleGridSpacing*M_PI/180;

      plotDriver->line(XCENT-RADIUS*cos(angle),YCENT-RADIUS*sin(angle),
                XCENT+RADIUS*cos(angle),YCENT+RADIUS*sin(angle) );
    }
}


void PlotPattern::plotOnePattern( AntennaPattern &pat, double offsetAmpl )
{
  double ampl = pat.getAmpl(0) + offsetAmpl;

  if ( ampl > maxAmpl ) ampl = maxAmpl;
  if ( ampl < minAmpl ) ampl = minAmpl;

  double angle = pat.getAngle(0)*M_PI/180;

  double radius = RADIUS*(ampl-minAmpl)/(maxAmpl-minAmpl);
  assert( radius >= 0 && radius <= RADIUS );

  plotDriver->moveTo( XCENT+radius*cos(angle), YCENT+radius*sin(angle) );

  for( int i=1; i< pat.size(); i++)
    {
     ampl = pat.getAmpl(i) + offsetAmpl;

     if ( ampl > maxAmpl ) ampl = maxAmpl;
     if ( ampl < minAmpl ) ampl = minAmpl;

     angle = pat.getAngle(i)*M_PI/180;

     radius = RADIUS*(ampl-minAmpl)/(maxAmpl-minAmpl);
     assert( radius >= 0 && radius <= RADIUS );

     plotDriver->lineTo( XCENT+radius*cos(angle), YCENT+radius*sin(angle) );
    }
}


void PlotPattern::plotAllPatterns()
{
  plotDriver->setColor( Plot::RED );

  if ( patternList.size() == 0 ) return;

  double amplOffset = -maxPatternListAmpl();

  for( int i=0; i< patternList.size(); i++ )
    {
     plotOnePattern( *patternList[i], amplOffset );


     double x = LEGEND_LEFT;
     double y = LEGEND_TOP - (i*(uncommonLabels.size()+1))*TEXT_VSPACING;

     plotDriver->line( x,y, x+ (LEGEND_INSET-.1), y);

     plotDriver->nextColor();
    }
}


void PlotPattern::plotTitle()
{
  plotDriver->setColor( Plot::BLACK );

  plotDriver->text( XCENT, 6.9, title, Plot::CENTERTOP );
}

void PlotPattern::plotDate() // setupLedgend() must have been called previously
{
  plotDriver->setColor( Plot::BLACK );

  plotDriver->text( 0, TEXT_VSPACING, plotTimeStr );
  plotDriver->text( 0, 0, plotDateStr );
}


void PlotPattern::plotAmplLabels()
{
  if (amplLabelSpacing == 0) return;

  plotDriver->setColor( Plot::BLACK );

  assert( amplLabelSpacing > 0 );

  int numCircles = int ( (maxAmpl-minAmpl)/amplLabelSpacing );
  assert( numCircles >= 0 );

  double spacing = amplLabelSpacing/(maxAmpl-minAmpl)*RADIUS;

  if (RADIUS - numCircles*spacing < TEXT_VSPACING) numCircles--;

  char str[80];

  sprintf( str, "%lg", maxAmpl);
  plotDriver->text( XCENT, YCENT-RADIUS, str, Plot::RIGHTBOTTOM );

  for(int i=0;i<numCircles;i++)
    {
     double dB = -(i+1)*amplLabelSpacing + maxAmpl;

     sprintf(str, "%lg", dB );

     plotDriver->text( XCENT, YCENT-RADIUS+ spacing*(i+1), str, Plot::RIGHTMIDDLE );
    }


  sprintf( str, "%lg", minAmpl);
  plotDriver->text( XCENT, YCENT, str, Plot::RIGHTMIDDLE );


  for(i=0;i<numCircles;i++)
    {
     double dB = -(numCircles-1-i+1)*amplLabelSpacing + maxAmpl;

     sprintf(str,"%lg", dB );

     plotDriver->text( XCENT, YCENT+RADIUS-(numCircles-1-i+1)*spacing , str, Plot::RIGHTMIDDLE );
    }

  sprintf( str, "%lg", maxAmpl );
  plotDriver->text( XCENT, YCENT+RADIUS , str, Plot::RIGHTTOP );
}


void PlotPattern::plotAngleLabels()
{
  if (angleLabelSpacing == 0) return;

  plotDriver->setColor( Plot::BLACK );

  assert( angleLabelSpacing>0);
  int numLines = int( 180.0/angleLabelSpacing );

  for(int i=0;i<numLines*2;i++)
    {
     double angle = (i-numLines+1)*angleLabelSpacing;

     char str[80];
     sprintf(str, "%lg", angle );

     plotDriver->rayText( XCENT, YCENT, RADIUS, angle*M_PI/180, str );
    }
}


void PlotPattern::plotLegend()  // setupLedgend() must have been called previously
{
  plotDriver->setColor( Plot::BLACK );

  for( int patNum = 0; patNum < patternList.size() ; patNum++)
    {
      for( int i=0; i< uncommonLabels.size() ; i++)
        {
          const char *label = patternList[patNum]->getLabel( uncommonLabels[i] );

          double x = LEGEND_LEFT+LEGEND_INSET;
          double y = LEGEND_TOP - (patNum*(uncommonLabels.size()+1)+i)*TEXT_VSPACING;

          plotDriver->text( x,y, label, Plot::LEFTMIDDLE );
        }
    }

  if (uncommonLabels.size() > 0 )
    plotDriver->rectangle( LEGEND_LEFT-.125, LEGEND_TOP + .125,
                  plotDriver->getMaxX(),
                  LEGEND_TOP - ((patternList.size()*(uncommonLabels.size()+1)-1)*TEXT_VSPACING) );

}


void PlotPattern::plotCommonInfoText()
{
  plotDriver->setColor( Plot::BLACK );

  for( int i=0; i< commonLabels.size() ; i++)
    {
     const char *label = patternList[0]->getLabel( commonLabels[i] );

     double x = LEGEND_LEFT;
     double y = (commonLabels.size()-1-i)*TEXT_VSPACING;

     plotDriver->text( x,y, label );
    }
}


double PlotPattern::maxPatternListAmpl()
{
  assert( patternList.size() > 0);

  double maxPatAmpl = patternList[0]->getMaxAmpl();

  for( int i=1; i< patternList.size(); i++)
    {
     double ampl = patternList[i]->getMaxAmpl();
     if ( ampl > maxPatAmpl)
       maxPatAmpl = ampl;
    }

  return maxPatAmpl;
}


void PlotPattern::setupLegend()
{

  struct tm *time_now;
  time_t secs_now;

  tzset();
  time(&secs_now);

  time_now = localtime(&secs_now);

  strftime(plotTimeStr, sizeof(plotDateStr) , "%I:%M %p", time_now );
  strftime(plotDateStr, sizeof(plotDateStr) , "%b %d %Y", time_now );

  commonLabels.clear();
  uncommonLabels.clear();


  if( patternList.size() >= 1 )
    {
     for(int i=0; i< NUM_ANTENNA_PATTERN_LABELS; i++)
       {
         enum {NO, YES} nonBlankLabelExists=NO, twoLabelsDifferent=NO;

         AntennaPattern::LabelIndex index = AntennaPattern::LabelIndex(i);

         for(int patNum=0; patNum < patternList.size(); patNum++)
           {
            if (strlen( patternList[patNum]->getLabel(index) ) > 0 )
              nonBlankLabelExists=YES;

            if ( patNum>0 && strcmp( patternList[patNum]->getLabel(index),
                                      patternList[0]->getLabel(index) ) )
              {
               twoLabelsDifferent=YES;
               break;
              }
           }

         if( index == AntennaPattern::DATE && twoLabelsDifferent == NO
              && !strcmp( patternList[0]->getLabel(index),plotDateStr ) )
               continue;

         if ( nonBlankLabelExists == YES )
           {
            if ( twoLabelsDifferent == NO && patternList.size() > 1)
              {
               commonLabels.addItem( index );
              }
            else
              {
               uncommonLabels.addItem( index );
              }
            }
       }
    }
}

double PlotPattern::calcAmplGridSpacing()
{
  assert( maxAmpl > minAmpl);

  return  amplGridSpacing/(maxAmpl-minAmpl)*RADIUS;
}

int PlotPattern::calcNumAngleLines()
{
 return int( 180.0/angleGridSpacing );
}
