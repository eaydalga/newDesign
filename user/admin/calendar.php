<?php
// PHP Calendar Control.
// Use the querystring as a Unix epoch number
// for our initial calendar display.
if(isset($_REQUEST['f'])) {
     $fform = $_REQUEST['f'];
     if(isset($_REQUEST['n'])) {
          $fname = $_REQUEST['n'];
     } else {
          exit();
     }
     if(isset($_REQUEST['ww'])) {
          $ww = $_REQUEST['ww'];
     } else {
          exit();
     }
     if(isset($_REQUEST['dts'])) {
          $qstring = $_REQUEST['dts'];
     } else {
          $qstring = '';
     }
     // If there is no querystring value,
     // use the first moment of the first
     // day of this month as the epoch number.
     if (empty ($qstring)){
             $ts = mktime (0, 0, 0, date ('n'), date('d'), date ('Y'));
     } else {
             // First make sure the querystring is numeric only.
             if (is_numeric($qstring)){
                     $ts = $qstring;
             } else {
                     // Some sort of querystring crack effort, perhaps.
                     // Shut it down, impolitely.
             exit();
             }
     }
} else {
     exit();
}
// Determine the month and year from querystring.
// Return the 3-letter month designator
// for the calendar title bar.
$month_alpha = date('M', $ts);
// Return the 1 or 2 digit month designator.
$month_num = date('n', $ts);
// Return the year as a 4 digit number.
$year = date('Y', $ts);
// Return the Unix epoch number for the first moment
// of the first day of this month.
$month_start_ts = mktime(0, 0, 0, $month_num, 1, $year);
// Return the day of the week designator
// (0=Sun, 1=Mon, etc.) for the first
// day of this month.
$weekday_first = date('w', $month_start_ts);
// Return the number of days in this month.
$days_in_month = date('t', $month_start_ts);
// Create the array of calendar cell contents.
$j = 1;
// Use a 'for loop' to load the array, beginning
// with the index that matches the weekday that
// the month begins with.
for ($i = 0; $i < 42; $i++){
        $arr_calendar[$i] = 0;
}
for ($i = $weekday_first; $j <= $days_in_month; $i++){
        $arr_calendar[$i] = mktime (0, 0, 0, $month_num, $j, $year);
        $j++;
}
// Next and Prior month querystring values for hyperlinks.
switch ($month_num){
        case 12:
                $next_month_ts = mktime(0, 0, 0, 1, 1, $year + 1);
                $prior_month_ts = mktime(0, 0, 0, 11, 1, $year);
                break;
        case 1:
                $next_month_ts = mktime(0, 0, 0, 2, 1, $year);
                $prior_month_ts = mktime(0, 0, 0, 12, 1, $year - 1);
                break;
        default:
                $next_month_ts = mktime(0, 0, 0, $month_num + 1, 1, $year);
                $prior_month_ts = mktime(0, 0, 0, $month_num - 1, 1, $year);
                break;
}
// Next and Prior year querystring values for annual hyperlinks.
$next_year_ts = mktime(0, 0, 0, $month_num, 1, $year + 1);
$prior_year_ts = mktime(0, 0, 0, $month_num, 1, $year - 1);
// Draw the calendars.
// Start the CSS font div tag.
print "<div class=\"gyazi\">";
print "<h3>TAKVİM</h3><fieldset>";
// Create the monthly embedded table. I'm using a
// 91% width because the table is embedded
// in another structure, and this will provide
// the calendar size I want.
print "<table width=\"95%\" class=\"byazi\">";
// The title bar, with 'prior month' and 'next month'
// hyperlinks, and month and year display.
print "<tr><td>";
print "</td><td>";
print "<a href=\"javascript:tarihbak('$fform','$fname','$ww','$prior_month_ts');\" class=\"nav\"><<</a></td>";
print "<td>$month_alpha</td>";
print "<td>";
print "<a href=\"javascript:tarihbak('$fform','$fname','$ww','$next_month_ts');\" class=\"nav\">>></a></td>";
print "<td>";
print "<a href=\"javascript:tarihbak('$fform','$fname','$ww','$prior_year_ts');\" class=\"nav\"><<</a></td>";
print "<td> $year</td>";
print "<td>";
print "<a href=\"javascript:tarihbak('$fform','$fname','$ww','$next_year_ts');\" class=\"nav\">>></a></td>";
// The top row, for days of the week.
print "</tr><tr>";
print "<td width=\"13%\" align=\"right\">Pz</a></td>";
print "<td width=\"13%\" align=\"right\">Pt</a></td>";
print "<td width=\"13%\" align=\"right\">Sa</a></td>";
print "<td width=\"13%\" align=\"right\">Ca</a></td>";
print "<td width=\"13%\" align=\"right\">Pe</a></td>";
print "<td width=\"13%\" align=\"right\">Cu</a></td>";
print "<td width=\"13%\" align=\"right\">Ct</a></td>";
print "</tr><tr>";
// Timestamp for the beginning moment of the current
// date, for use in possibly adding a background color.
$today_ts = mktime(0, 0, 0, date('n'), date('j'), date('Y'));
$like_ts = mktime(0, 0, 0, $month_num, date('j'), $year);
// Create the individual date cells.
for ($i = 0; $i < 42; $i++){
        // Change the background color of the cell to yellow if
        // it matches the current date. Change it to blue if
        // it matches the date selected. Yellow has precedence
        // if both conditions apply.
        if($arr_calendar[$i] == $today_ts){
                print "<td width=\"13%\" bgcolor=\"#FFFF00\" align=\"right\">";
        } elseif ($arr_calendar[$i] == $like_ts){
                print "<td width=\"13%\" bgcolor=\"#BCD2EE\" align=\"right\">";
        } else {
                print "<td width=\"13%\" align=\"right\">";
        }
        // Only print the array value when it contains a Unix epoch number.
        if ($arr_calendar[$i] != 0){
		// Create the anchor tag and url, and use the epoch number
		// as the querystring.
                $tarih = date('Ymd', $arr_calendar[$i]);
                print "<a href=\"javascript:tarihal('$fform','$fname','$tarih');\" class=\"nav\" >";
		// Determine the date for this array value
		// and write it to the cell.
                print date('j', $arr_calendar[$i]);
		// Close the anchor tag.
                print "</a>";
        }
        print "</td>";
        // At the end of each week of the calendar, start a new row.
        switch ($i){
                case 6:
                        print "</tr><tr>";
                        break;
                case 13:
                        print "</tr><tr>";
                        break;
                case 20:
                        print "</tr><tr>";
                        break;
                case 27:
                        print "</tr><tr>";
                        break;
                case 34:
                        print "</tr><tr>";
                        break;
        }
}
// Close the monthly embedded table.
print "</tr></table>";
// Close the font div tag.
print "</fieldset>";
print "</div>";
?>
