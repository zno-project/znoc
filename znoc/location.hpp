#ifndef _LOCATION_H
#define _LOCATION_H

struct SourceLocation {
  int Line;
  int Col;
};
extern SourceLocation LexLoc;

#endif