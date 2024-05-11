Attribute VB_Name = "SDXF"
Option Compare Database
Option Explicit


Public Declare Function sdxfB _
   Lib "D:\projects\SDXF\SDXF.dll" _
   (parm As TSDX_param, ByVal data As String) As Long




Public Type TSDX_param
  magic      As String * 4      ' C A   4  000 magic for test must be "PINP"
  handle     As Long            ' F B   4  004 internal
  value      As Long            ' F I   4  008 binary value
  length     As Long            ' F I   4  012 data length
  bufferSize As Long            ' F I   4  016 size of SDXF container
  chunkID    As Integer         ' H I   2  020 chunk ID
  dataType   As Integer         ' H I   2  022 SDXF data type
  function   As Integer         ' H I   2  024 one of f_
  rc         As Integer         ' H I   2  026 return code
  ec         As Integer         ' H I   2  028 extended rc
  level      As Integer         ' H I   2  030 SDXF struct level
  do_encrypt As Byte            ' X I   1  032 = 0 or 1
  compr      As Byte            ' X I   1  033 compression method
  option     As String * 1      ' C A   1  034 fileopen: 'R' or 'W'
  name       As String * 32     ' C A  32  036 depends on function:
                                '              open_server, connect: server name
                                '              get_global, set_userID: userID
                                '              set_password: password
                                '              get_version : version string
                                '              set_appl_key: application key
  passw      As String * 16     ' C A  16  068 out: actual function name
End Type




Public Enum SDXF_Function
  f_sdx_get_version = 0 ' out: NAME: version string                 */
  f_sdx_init_old        ' in OPTION: old='O', new='N'               */
  f_sdx_init_new        ' !!! obsolete by init! */
  f_sdx_enter           ' out: CHUNK-ID                             */
  f_sdx_leave           ' out: CHUNK-ID                             */
  f_sdx_next            ' out: CHUNK-ID                             */
  f_sdx_select          ' in: CHUNK-ID                              */
  f_sdx_extract         ' in: MAX-LENGTH, out: DATA                 */
  f_sdx_create          ' in: LENGTH, CHUNK-ID, DATA,               */
                          '     DATA-TYPE, DO-ENCRYPTION              */
  f_sdx_append          ' in: DATA                                  */
  f_sdx_adapt_short     ' inout: VALUE                              */
  f_sdx_adapt_long      ' inout: VALUE                              */
  f_sdx_get_buffer      ' in: LENGTH (allocates memory)             */
  f_sdx_read_chunk      ' in: MAX-LENGTH, DATA                      */
  f_sdx_write_chunk     ' in: MAX-LENGTH, out: DATA                 */
  f_sdx_file_open       ' in: NAME, OPTION ('R', 'W')               */
  f_sdx_file_close
  f_sdx_set_encryption  ' get DO-ENCRYPTION from config setting     */
  f_sdx_get_err_text    ' out: DATA: text interpr. rc, ec           */
  f_sdx_set_password    ' client password for authorization         */
End Enum

Private Enum SDX_DataType
  SDX_DT_inconsistent = 0     '  0x00 */
  SDX_DT_structured = 1       '  0x20 */
  SDX_DT_binary = 2           '  0x40 */
  SDX_DT_numeric = 3          '  0x60 */
  SDX_DT_char = 4             '  0x80 */
  SDX_DT_float = 5            '  0xA0 */ '  IEEE 754 */
  SDX_DT_UTF8 = 6             '  0xC0 */
  SDX_DT_implemented = 6      '  highest known DT number */
End Enum

Enum SDX_RC   ' return codes
  SDX_RC_ok = 0
  SDX_RC_failed = 1
  SDX_RC_warning = 1
  SDX_RC_illegalOperation = 2
  SDX_RC_dataError = 3
  SDX_RC_parameterError = 4
  SDX_RC_programError = 5
  SDX_RC_noMemory = 6
End Enum


Private Enum SDX_EC    ' extended return codes
  SDX_EC_ok = 0
  SDX_EC_eoc = 1
  SDX_EC_notFound = 2
  SDX_EC_dataCutted = 3
  SDX_EC_overflow = 4
  SDX_EC_wrongInitType = 5
  SDX_EC_comprerr = 6
  SDX_EC_forbidden = 7
  SDX_EC_unknown = 8
  SDX_EC_levelOvflw = 9
  SDX_EC_paramMissing = 10
  SDX_EC_magicError = 11
  SDX_EC_not_consistent = 12
  SDX_EC_wrongDataType = 13
  SDX_EC_noMemory = 14
  SDX_EC_malformed = 15
  SDX_EC_crypterror = 16
  SDX_EC_wrongLength = 17
  SDX_EC_FTP_basic = 17
  SDX_EC_FTP_inexact = 18
  SDX_EC_FTP_invalid = 19
  SDX_EC_FTP_underflow = 20
  SDX_EC_FTP_overflow = 21
  SDX_EC_unknown_function = 22
  SDX_EC_highestNumber = 29
  SDX_EC_error = 99
End Enum

Enum Tel_eintrag
  TE_root = 4711
  TE_eintrag
  TE_name
  TE_telefon
  TE_anschrift
  TE_email
End Enum

Sub stest()
  Const P_tab = "Telefonverz"
  Const ausgabe = "D:\projects\SDXF\telver.sdx"

  Static par As TSDX_param
  Dim data As String
  Dim rs As Object
  Dim anschrift As Variant
  
  On Error GoTo 0
  par.rc = 0
  Set rs = CurrentDb.OpenRecordset _
    ("select * from " & P_tab & " order by name;")
  
  par.function = f_sdx_get_version
  sdxfB par, data
  check par, 0
  
  par.function = f_sdx_init_new
  par.bufferSize = 120000
  sdxfB par, vbNullString
  check par, 1
  
  par.chunkID = TE_root
  par.function = f_sdx_create
  par.dataType = SDX_DT_structured
  sdxfB par, vbNullString
  check par, 2
  
  Do While Not rs.EOF
    If par.rc > 0 Then
      Err.Raise 6666, , "error  for " & rs!name & ", rc=" & par.rc & ", ec=" & par.ec & _
        " on " & par.name
      Exit Do
    End If
    
    If IsNull(rs!telefon) Then GoTo cont
    
    par.chunkID = TE_eintrag
    par.function = f_sdx_create
    par.dataType = SDX_DT_structured
    sdxfB par, vbNullString
    check par, 11
    
    par.chunkID = TE_name
    data = Split(rs!name, vbNewLine, 2)(0)
    par.dataType = SDX_DT_char
    par.length = Len(data)
    sdxfB par, data
    check par, 12
    
    par.chunkID = TE_telefon
    data = Split(rs!telefon, vbNewLine, 2)(0)
    par.length = Len(data)
    sdxfB par, data
    check par, 13
    
        
    par.dataType = SDX_DT_structured
    par.chunkID = TE_anschrift
    sdxfB par, vbNullString
    check par, 14
    par.dataType = SDX_DT_char
    
    If Not IsNull(rs!anschrift) Then
      For Each anschrift In Split(rs!anschrift, vbNewLine)
        par.length = Len(anschrift)
        If par.length = 0 Then Exit For
        sdxfB par, anschrift
        check par, 15
      Next
    End If
  
    par.function = f_sdx_leave
    sdxfB par, vbNullString
    check par, 16
    
    If Not IsNull(rs!email) Then
      par.chunkID = TE_email
      par.function = f_sdx_create
      par.dataType = SDX_DT_char
      par.length = Len(rs!email)
      sdxfB par, rs!email
      check par, 17
    End If
  
    par.function = f_sdx_leave
    sdxfB par, vbNullString
    check par, 18, data
cont:
    rs.MoveNext
  Loop
  
  par.function = f_sdx_leave
  sdxfB par, vbNullString
  check par, 3
  
  par.function = f_sdx_file_open
  par.option = "W"
  par.name = ausgabe
  sdxfB par, vbNullString
  check par, 4
  
  par.function = f_sdx_write_chunk
  sdxfB par, vbNullString
  check par, 5
  
  par.function = f_sdx_file_close
  sdxfB par, vbNullString
  check par, 6
  
  
  ' MsgBox data
End Sub

Sub check(ByRef par As TSDX_param, bm As Integer, Optional data As String)
    If par.rc > 0 Then
      Err.Raise 7777, , "error  rc=" & par.rc & _
        ", ec=" & par.ec & ", at " & bm & _
        vbNewLine & "with '" & data & "' on " & par.name
    End If

End Sub
