#ifndef ANTPAT_H
#define ANTPAT_H

#include <fstream.h>
#include "avector.h"
#include <time.h>

#define NUM_ANTENNA_PATTERN_LABELS 10


class AntennaPattern
{
public:

  AntennaPattern();
  AntennaPattern( const char *fileName);
  AntennaPattern( const AntennaPattern &pat);
  ~AntennaPattern();

//  AntennaPattern(Vector<double> &freq, Vector<double> &ampl);

  int     size() const;
  double  getAngle(int index) const;
  double  getAmpl (int index) const;

  void    setAngle(int index, double angle);
  void    setAmpl (int index, double ampl );

  double  getMaxAmpl() const;

  enum LabelIndex { FILE_NAME, ANTENNA, FREQUENCY, PLANE, POLARIZATION,
                    BEAMWIDTH, COMMENT, DETECT_MODE, SLL, DATE,
                    UNKNOWN_LABEL=-1 };

  const char*   getLabelName(LabelIndex i ) const;
  const char*   getLabel    (LabelIndex i ) const;
  void          setLabel    (LabelIndex i, const char *str );

  int           getNumLabels() const;
  void          clearLabels(); //doesn't clear filename


  void write( ostream &stream = cout) const;
  void write( const char *fileName ) const;

  void read( istream &stream);
  void read( const char *fileName);

  double beamwidth( double *left_angle=0, double *right_angle=0) const;
  double beamwidth( double level_dB,
                    double *left_angle=0, double *right_angle=0 ) const;

  AVector<double> angleVec;
  AVector<double> amplVec;

protected:
  Vector<char> label[NUM_ANTENNA_PATTERN_LABELS];

private:
  LabelIndex    findLabel   (const char *labelName ) const;

};

#endif