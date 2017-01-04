#ifndef plot_h
#define plot_h

class Plot
{
public:

  enum DrawColor   { NONE, BLACK, BLUEGREEN, GREEN, RED, BLUE, PURPLE };
  enum TextJustify { CENTERTOP, CENTERMIDDLE, CENTERBOTTOM, LEFTTOP, LEFTMIDDLE, LEFTBOTTOM, RIGHTTOP, RIGHTMIDDLE, RIGHTBOTTOM};


  Plot();
  virtual ~Plot() {};

  void move  ( double x, double y );
  void moveTo( double x, double y );

  void line  ( double x, double y );
  void lineTo( double x, double y );

  void line  ( double x1, double y1,
               double x2, double y2  );

  void text    ( double x, double y, const char *str, TextJustify justify=LEFTBOTTOM);

  void rayText ( double center_x, double center_y,
                 double radius, double ang, const char *str );


  void rectangle( double x1, double y1, double x2, double y2);
  void circle   ( double x, double y, double radius );

  void setColor( DrawColor color );
  void nextColor();

  virtual double getMaxX() = 0;
  virtual double getMaxY() = 0;

  virtual double getLetterHeight() = 0;
  virtual double getLetterWidth()  = 0;


protected:

  int curX;
  int curY;
  DrawColor penColor; // actual pen color held by plotter arm
  DrawColor curColor; // current color to draw next line as

  enum UpDown { UP, DOWN };
  UpDown curUpDown;

  virtual int mapX( double x_inch ) = 0;
  virtual int mapY( double y_inch ) = 0;

  virtual void penUp() = 0;
  virtual void penDown() = 0;

          void penMove  ( int x, int y );
  virtual void penMoveTo( int x, int y ) = 0;

  virtual void penText  ( int x, int y, const char *str, TextJustify justify = LEFTBOTTOM ) = 0;

  virtual void setPenColor( DrawColor color ) = 0;

private:
  double rayTextRadiusCalc( double r, double th, double vx, double vy );
  double max( double a, double b);

};


#endif