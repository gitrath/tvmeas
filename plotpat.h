#ifndef PLOTPAT_H
#define PLOTPAT_H

#include "plot.h"
#include "antpat.h"
#include "list.h"

class PlotPattern
{
public:
  PlotPattern( Plot *plot_driver );

  void setTitle ( const char *title_string );

  void setMinMax( double min_ampl_dB = -30, double max_ampl_dB = 0);

  void addPattern( const AntennaPattern *pat );

  void setAngleGrid ( double spacing_deg = 10 );
  void setAmplGrid  ( double spacing_dB  = 3 );

  void setAngleLabels ( double spacing_deg = 30 ); // set to zero for NO labels
  void setAmplLabels  ( double spacing_dB  = 6  ); // set to zero for NO labels

  void plot();


protected:

  List<AntennaPattern *> patternList;

  Vector<char> title;

  double minAmpl;
  double maxAmpl;

  double angleGridSpacing;
  double amplGridSpacing;

  double angleLabelSpacing;
  double amplLabelSpacing;

  Plot   *plotDriver;

  void plotAmplGrid();
  void plotAngleGrid();
  void plotOnePattern( AntennaPattern &pat, double amplOffset );
  void plotAllPatterns();
  void plotTitle();
  void plotDate();
  void plotAmplLabels();
  void plotAngleLabels();
  void plotLegend();
  void plotCommonInfoText();

  double maxPatternListAmpl();

  void setupLegend();

private:

  List< AntennaPattern::LabelIndex > uncommonLabels,commonLabels;

  double calcAmplGridSpacing();
  int    calcNumAngleLines();

  char  plotTimeStr[20];
  char  plotDateStr[20];

};


#endif