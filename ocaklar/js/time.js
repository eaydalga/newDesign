function CheckTime(itm) {
  //Copyright © 2002 Booshway Enterprises, Inc. all rights reserved v1.0
  var err=0
  a=tm;
  if (a.length != 8) err=1
  f = a.substring(0, 2)  // Hour f
  c = a.substring(2, 3)  // ':'
  b = a.substring(3, 5)  // Min b
  e = a.substring(5, 6)  // ':'
  d = a.substring(6, 8)  // Sec d
  if (b<0 || b>59) err = 1
  if (d<0 || d>59) err = 1
  if (f<0 || f>23) err = 1
  if (err==1) {
    alert("time" + ": " + tm + ' is not a valid time. Please re-enter. Format HH:MM:SS.');
  }
  return err;
}
