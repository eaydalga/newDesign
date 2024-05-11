function Validate_String(string, return_invalid_chars)
{
    valid_chars = '1234567890-_.^~abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ';
    invalid_chars = '';

    if(string == null || string == '')
         return(true);

    //For every character on the string.   
    for(index = 0; index < string.length; index++)
    {
         char = string.substr(index, 1);                        
         //Is it a valid character?
         if(valid_chars.indexOf(char) == -1)
             {
             //If not, is it already on the list of invalid characters?
             if(invalid_chars.indexOf(char) == -1)
                 {
                 //If it's not, add it.
                 if(invalid_chars == '')
                      invalid_chars += char;
                 else
                      invalid_chars += ', ' + char;
                 }
              }
    }                     
    //If the string does not contain invalid characters,
    //   the function will return true.
    //If it does, it will either return false or a list of
    //   the invalid characters used
    //in the string, depending on the value of the second parameter.
    if(return_invalid_chars == true && invalid_chars != '')
         {
         last_comma = invalid_chars.lastIndexOf(',');
             
         if(last_comma != -1)
                 invalid_chars = invalid_chars.substr(0, $last_comma) + 
                 ' and ' + invalid_chars.substr(last_comma + 1, invalid_chars.length);

         return(invalid_chars);
         }
    else
         return(invalid_chars == ''); 
}
function Validate_Email_Address(email_address)
{
    //Assumes that valid email addresses consist of user_name@domain.tld
    at = email_address.indexOf('@');
    dot = email_address.indexOf('.');

    if(at == -1 || 
       dot == -1 || 
       dot <= at + 1 ||
       dot == 0 || 
       dot == email_address.length - 1)
           return(false);

    user_name = email_address.substr(0, at);
    domain_name = email_address.substr(at + 1, email_address.length);

    if(Validate_String(user_name) === false || 
       Validate_String(domain_name) === false)
           return(false);                     

    return(true);
}
