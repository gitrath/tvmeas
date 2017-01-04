#include <iostream.h>
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <string.h>
#include "receiver.h"  

// Static variables of Receiver Class
double Receiver::freq;


double Receiver::setFreq(double f)
{
 freq = f;

 return f;
}

double Receiver::getAmpl()
{
 assert(!"Default Receiver getAmpl() not yet defined");
 return 0;
}



// Static variables of SpecAnalyzer Class:
int     SpecAnalyzer::hasBeenInitialized = 0; // yes=1, no=0
double  SpecAnalyzer::attenuation;
double  SpecAnalyzer::span;
double  SpecAnalyzer::videoBW;
double  SpecAnalyzer::resolutionBW;
double  SpecAnalyzer::sweepTime;
double  SpecAnalyzer::lastSearchFreq = 0; // zero means no search taken place yet
clock_t SpecAnalyzer::theTime;
GPIB    SpecAnalyzer::gpib("SPECTR");

SpecAnalyzer::TriggerMode   SpecAnalyzer::triggerMode;
SpecAnalyzer::DetectionMode SpecAnalyzer::detectionMode;


#define MINZOOMSPAN  .1e6


SpecAnalyzer::SpecAnalyzer()
{
  if (!hasBeenInitialized)
    {
     hasBeenInitialized = 1; // yes = 1, no = 0

     //wait(3); // probably need???
     setAttenuation(0);         // set input attenuation
     setSpan (19.25e9);         // set span
     setVBW  (1e6);             // set video bandwidth
     setRBW  (1e6);             // set resolution bandwidth
     setSweepTime( .4 );        // set sweep time
     setTriggerMode( FREE );    // set trigger mode to free-run mode
     setDetectionMode(SAMPLE);  // set detection mode

     freq = 0;
     setFreq(12.38e9);

    }

}

double SpecAnalyzer::setFreq(double f)
{
  if (freq != f)
    {
     sendCmd("cf",f,"hz");
     freq = readNum("cf?");
    }

  return freq;
}

double SpecAnalyzer::getAmpl()
{
  sendCmd("ts");
  waitTilDone();

  sendCmd("mkpk");

  return gpib.readNum("mka?;");
}

void SpecAnalyzer::waitTilDone()
{
  double done;
  int    n;

  gpib.write("done?;");

  n = sscanf(gpib.slowRead(),"%lg",&done);

  assert( done == 1);
  assert( n == 1   );

}

void SpecAnalyzer::local()
{
  gpib.local();
}


void SpecAnalyzer::wait(double t)
{
  resetTime();
  while( time() < t );
}


void SpecAnalyzer::resetTime()
{
  theTime = clock();
}

double SpecAnalyzer::time()     // seconds since last resetTime() call
{
  return (clock()-theTime)/double(CLK_TCK);
}


double SpecAnalyzer::setRBW(double RBW)
{
  sendCmd ("rb",RBW,"hz");
  resolutionBW = readNum("rb?");
  return resolutionBW;
}

double SpecAnalyzer::setVBW(double VBW)
{
  sendCmd ("vb",VBW,"hz");
  videoBW = readNum("vb?");
  return videoBW;
}


void SpecAnalyzer::turnDisplayOff()
{
  sendCmd ("grat off");
}


void SpecAnalyzer::turnDisplayOn()
{
  sendCmd ("grat on");
}


void SpecAnalyzer::setSingleSweep()
{
  sendCmd ("sngls");
}


void SpecAnalyzer::setAutoSweep()
{
  sendCmd ("conts");
}


void SpecAnalyzer::setTriggerMode( TriggerMode mode )
{
  switch (mode)
    {
     case EXTERNAL:
       sendCmd("tm ext");
       break;
     case FREE:
       sendCmd("tm free");
       break;
     default:
       abort();
    }
  triggerMode = mode;
}


void SpecAnalyzer::setDetectionMode( DetectionMode mode )
{
 switch (mode)
   {
    case SAMPLE:
      sendCmd("det smp");
      break;
    case POSITIVE:
      sendCmd("det pos");
      break;
    case NEGATIVE:
      sendCmd("det neg");
      break;
    default:
      abort();
   }
 detectionMode = mode;
}

double SpecAnalyzer::setSweepTime(double t)
{
  sendCmd ("st",t,"s");
  sweepTime = readNum("st?");
  return sweepTime;
}

double SpecAnalyzer::setSpan(double span_hz)
{
  sendCmd ("sp",span_hz,"hz");
  span = readNum("sp?");
  return span;
}

double SpecAnalyzer::setAttenuation(double atten_dB)
{
  sendCmd ("at",atten_dB,"db");
  attenuation = readNum("at?");
  return attenuation;
}

double SpecAnalyzer::setRefLevel(double ref_level_dBm)
{
  sendCmd ("rl",ref_level_dBm,"dbm");

  return readNum("rl?");
}

double SpecAnalyzer::searchFreq (double initFreq, double initSpan )
{
  assert( initFreq > 0 );
  assert( initSpan > 0 );

  assert( initFreq - initSpan/2 > 0 );
  assert( getFreq() - initSpan/2 > 0 );

  lastSearchFreq = initFreq;

  double      vbw_save         = getVBW();
  double      rbw_save         = getRBW();
  double      sweepTime_save   = getSweepTime();
  TriggerMode triggerMode_save = getTriggerMode();

  setSingleSweep();
  setTriggerMode( FREE );
  sendCmd("rb auto");
  sendCmd("vb auto");
  setSweepTime( .4 );


  double sp = setSpan( initSpan );
  setFreq( initFreq );

  while ( sp > MINZOOMSPAN )
    {
     sendCmd ("ts");
     waitTilDone();
     sendCmd ("mkpk; mkcf");
     sp /= 10;
     sp = setSpan (sp);
    }

  sendCmd ("mkpk; mkcf");

  setSpan( 0 );
  setRBW( rbw_save );
  setVBW( vbw_save );
  setSweepTime( sweepTime_save );
  setTriggerMode( triggerMode_save );
  setAutoSweep();

  freq = readNum("cf?");

  return freq;

}


double SpecAnalyzer::getLastSearchFreq()
{
  return lastSearchFreq;
}


void  SpecAnalyzer::sendCmd (const char *cmd, double x, const char *units)
{
  char *cmdStr = new char[strlen(cmd) + strlen(units) + 20 + 3 + 1]; assert(cmdStr);
  assert (cmdStr != 0);

  int n = sprintf(cmdStr,"%s %lg %s;",cmd,x,units);
  assert(n>1);

  gpib.write(cmdStr);

  delete cmdStr;

  checkError();

}


void  SpecAnalyzer::sendCmd(const char *cmd)
{
  const int STRLEN = strlen(cmd)+1+1;

  char *cmdStr = new char[STRLEN]; assert( cmdStr );

  int n = sprintf(cmdStr,"%s;",cmd);
  assert( n < STRLEN);

  gpib.write(cmdStr);

  delete cmdStr;

  checkError();
}



double SpecAnalyzer::readNum(const char *cmd)
{
  const int STRLEN = strlen(cmd)+1+1;

  char *cmdStr = new char[STRLEN];  assert(cmdStr);

  int n = sprintf(cmdStr,"%s;",cmd);
  assert( n < STRLEN);

  gpib.write(cmdStr);

  delete cmdStr;

  double x = gpib.readNum();

  checkError();

  return x;

}

double SpecAnalyzer::getFreq()
{
  assert( freq > 0);
  return freq;
}

double SpecAnalyzer::getRBW()
{
  return resolutionBW;
}

double SpecAnalyzer::getVBW()
{
  return videoBW;
}

double SpecAnalyzer::getSweepTime()
{
  return sweepTime;
}

SpecAnalyzer::DetectionMode SpecAnalyzer::getDetectionMode()
{
  return detectionMode;
}

SpecAnalyzer::TriggerMode SpecAnalyzer::getTriggerMode()
{
  return triggerMode;
}


void SpecAnalyzer::readTrace( Vector<double> &amplVec )
{

  sendCmd("aunits dbm");  // should be default already


  #ifdef SPECTRUM_ANALYZER_MODEL_HP8562A
    double refLevel =  readNum("rl?");
    double scale    =  readNum("lg?");
  #endif

  sendCmd("tdf b");  // specifies binary data format (faster than reading ASCII)

  gpib.write("tra?;");

  unsigned char *traceA = (unsigned char *) gpib.slowRead( 1202 );

  amplVec.setSize( TRACEPOINTS );

  for(int i=0;i<TRACEPOINTS;i++)
    {
     #ifdef SPECTRUM_ANALYZER_MODEL_HP8562A
       double amplitude = refLevel +
    	  scale*( (traceA[i*2+1]+traceA[i*2]*256)/60. - 10. );
     #else  /* assume HP8593A */
       double amplitude = (traceA[i*2+1]+traceA[i*2]*256)*.01 - 90;
     #endif

     amplVec[i] = amplitude;
    }

}


void SpecAnalyzer::checkError()
{

  int errNum;

  gpib.write("err?;");

  int n = sscanf( gpib.read() ,"%d", &errNum);
  assert(n == 1);  // Caution: err? may return more than one error code in a comma separated list, this subroutine only reads the first error

  if (errNum == 0) return;

  char *str1, *str2;

  switch (errNum)
    {
     case 100:
       str1 = "NO PWRON";
       str2 = "Power-on state not valid; a dafault state was loaded instead";
       break;
     case 101:
       str1 = "NO STATE";
       str2 = "Recalled state not valid or not saved";
       break;
     case 106:
       str1 = "ABORTED!";
       str2 = "Aborted operation";
       break;
     case 107:
       str1 = "HELLO ??";
       str2 = "No HP-IB listener on bus";
       break;
     case 108:
       str1 = "TIME OUT";
       str2 = "Controller time out";
       break;
     case 109:
       str1 = "CTRLFAIL";
       str2 = "Take control of HP-IB failed";
       break;
     case 110:
       str1 = "NOT CTRL";
       str2 = "Not a controller";
       break;
     case 111:
       str1 = "# ARGMTS";
       str2 = "Command does not have enough arguments";
       break;
     case 112:
       str1 = "??CMD??";
       str2 = "Command not recognized";
       break;
     case 113:
       str1 = "FREQ NO!";
       str2 = "Command cannot have frequency units";
       break;
     case 114:
       str1 = "TIME NO!";
       str2 = "Command cannot have time units";
       break;
     case 115:
       str1 = "AMPL NO!";
       str2 = "Command cannot have amplitude units";
       break;
     case 116:
       str1 = "?UNITS??";
       str2 = "Units not recognized";
       break;
     case 117:
       str1 = "NOP NUM";
       str2 = "Command cannot have numeric units";
       break;
     case 118:
       str1 = "NOP EP";
       str2 = "Enable parameter cannot be used";
       break;
     case 119:
       str1 = "NOP UPDN";
       str2 = "Up/down are not valid arguments for the command";
       break;
     case 120:
       str1 = "NOP ONOF";
       str2 = "ON/off are not valid arguments for the command";
       break;
     case 121:
       str1 = "NOP ARG";
       str2 = "Auto/man are not valid arguments for the command";
       break;
     case 122:
       str1 = "NOP TRC";
       str2 = "Trace register not valid for the command";
       break;
     case 123:
       str1 = "NOP ABLK";
       str2 = "A-black not valid for the command";
       break;
     case 124:
       str1 = "NOP IBLK";
       str2 = "I-block not valid for the command";
       break;
     case 125:
       str1 = "NO STRNG";
       str2 = "Strings are not valid for the command";
       break;
     case 126:
       str1 = "NO ?";
       str2 = "Query is not allowed for the command";
       break;
     case 127:
       str1 = "BAD DTMD";
       str2 = "Detector mode is not valid";
       break;
     case 128:
       str1 = "PK WHAT?";
       str2 = "Peak-search parameter is not valid";
       break;
     case 129:
       str1 = "PRE TERM";
       str2 = "Premature termination";
       break;
     case 130:
       str1 = "BAD TDF";
       str2 = "Arguments are valid only for the TDF command";
       break;
     case 131:
       str1 = "?? AM/FM";
       str2 = "AM/FM are not valid arguments for the command";
       break;
     case 132:
       str1 = "!FAV/RMP";
       str2 = "FAV/RAMP are not valid arguments for the command";
       break;
     case 133:
       str1 = "!INT/EXT";
       str2 = "INT/EXT are not valid arguments for the command";
       break;
     case 134:
       str1 = "??? ZERO";
       str2 = "ZERO is not a valid argument for the command";
       break;
     case 135:
       str1 = "??? CURR";
       str2 = "CURR is not a valid argument for the command";
       break;
     case 136:
       str1 = "??? FULL";
       str2 = "FULL is not a valid argument for the command";
       break;
     case 137:
       str1 = "??? LAST";
       str2 = "LAST is not a valid argument for the command";
       break;
     case 138:
       str1 = "!GRT/DSP";
       str2 = "GRT/DSP is not a valid argument for the command";
       break;
     case 139:
       str1 = "PLOTONLY";
       str2 = "Use argument only with the PLOT command";
       break;
     case 140:
       str1 = "?? PWRON";
       str2 = "PWRON is not a valid argument for the command";
       break;
     case 141:
       str1 = "BAD ARG";
       str2 = "Argument is valid only for the FDIAG command";
       break;
     case 142:
       str1 = "BAD ARG";
       str2 = "Query is expected for the FDIAG command";
       break;
     case 143:
       str1 = "NO PRESL";
       str2 = "Preselector hardware is required";
       break;
     case 144:
       str1 = "NEXT 44";
       str2 = "Next error space";
       break;
     default:
       str1 = "UNKNOWN ERROR";
       str2 = "An error not listed in Appendix B of the HP 8562A/B manual occured";
    }

  cerr << "**** HP8562 Spectrum Analyzer Error ****" << endl;
  cerr << "Error Code:  " << errNum << "  " << str1 << endl;
  cerr << str2 << endl;

  abort();

}