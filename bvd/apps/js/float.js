/*$RCSfile: validateFloat.js,v $ $Revision: 1.11 $ $Date: 2004/03/28 16:53:21 $ */
/**
  * Check to see if fields are a valid float.
  * Fields are not checked if they are disabled.
  * <p>
  * @param form The form validation is taking place on.
*/
function validateFloat(value1) {
    var bValid = true;

    var value = value1;
    if (value.length > 0) {
       // remove '.' before checking digits
       var tempArray = value.split('.');
       //Strip off leading '0'
       var zeroIndex = 0;
       var joinedString= tempArray.join('');
       while (joinedString.charAt(zeroIndex) == '0') {
           zeroIndex++;
       }
       var noZeroString = joinedString.substring(zeroIndex,joinedString.length);
       if (!isAllDigits(noZeroString)) {
           bValid = false;
           if (i == 0) {
               focusField = field;
           }
           fields[i++] = oFloat[x][1];
       } else {
           var iValue = parseFloat(value);
           if (isNaN(iValue)) {
               if (i == 0) {
                   focusField = field;
               }
               fields[i++] = oFloat[x][1];
               bValid = false;
           }
       }
    }
    return bValid;
}
