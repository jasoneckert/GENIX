!
! remind.p: version 1.2 of 11/30/82
! Mesa Unix System Command Source File
!
! @(#)remind.p	1.2 (NSC) 11/30/82
!
program PRemind (input, infile, output);






! This program reads a line from a 'reminder' file, which consists of lines
! each of which begin with a date.  The program checks whether the date
! in the line is today, and if so writes the line to standard output.

! The date must be the first item in the line and must be in month, day,
! year order.





! import '/a/linda/work/remind/dm.x';

!The following records define the time information as returned from UNIX,
!and the date and time record this module returns.  The Unix record
!contains the time in seconds since 00:00:00 Jan 1, 1970 Greenwich Mean Time,
!additional precision information, the time zone in minutes west of Greenwich,
!and a flag which indicates whether or not daylight savings time is applicable.

int = -32768..32767;

!The date record contains the day of the year (from 0), the year, the month
!the day of the month, the day of the week and hours, minutes, and seconds.

DateRec = record
	Sec: integer;
	Min: integer;
	Hour: integer;
	MDay: integer;
	Month: integer;
	Year: integer;
	WkDay: integer;
	YDay: integer;
	IsDST: integer;
end;

PDateRec = ^ DateRec;

import function time (var tloc: integer): integer; Ccall forward;
import function localtime (var clock: integer) tm: PDateRec; Ccall forward;

DateLineRange = 1..45;

DateLineType = packed array [DateLineRange] of char;

Century = 1900;

CurDate: DateRec;	!Used to store the current date gotten from the system

! This routine gets the time from the system (in seconds), and converts it
! to a date.

procedure GetTime (var LTime: integer);

var TmpTime: integer;

begin !GetTime
  LTime := 0;
  TmpTime := Time(LTime);
end; !GetTime

procedure BuildCurDate (LTime: integer; var Date: DateRec);

var CDate: PDateRec;

begin !BuildCurDate
  CDate := localtime(LTime);
  Date := CDate^;
! writeln('got month: ', Date.Month, ' Day: ', Date.MDay, ' Year: ', Date.Year);
end; !BuildCurDate

StrSize = 1..11;

StrType = packed array [StrSize] of char;

procedure CatString (
	S : StrType;
	SLen: StrSize;
	var Date : DateLineType;
	var Ndx : integer );

var I: integer;

begin
  for I := 1 to SLen do begin Date[Ndx] := S[I]; Ndx := Ndx + 1; end;
end;

procedure CatNum (
	Num: integer;
	var Date: DateLineType;
	var Ndx: integer );

var
  NumLeft,
  Digit: integer;

begin !CatNum
  NumLeft := Num div 10;
  Digit := Num mod 10;
  if NumLeft > 0 then CatNum(NumLeft, Date, Ndx);
  Date[Ndx] := chr(Digit + 48);
  Ndx := Ndx + 1;
end; !CatNum
     
procedure AddWkday (
	ref D: DateRec;
	var Date: DateLineType;
	var Ndx: integer );

begin
  case D.WkDay of
    0: CatString('Sunday,    ', 8, Date, Ndx);
    1: CatString('Monday,    ', 8, Date, Ndx);
    2: CatString('Tuesday,   ', 9, Date, Ndx);
    3: CatString('Wednesday, ', 11, Date, Ndx);
    4: CatString('Thursday,  ', 10, Date, Ndx);
    5: CatString('Friday,    ', 8, Date, Ndx);
    6: CatString('Saturday,  ', 10, Date, Ndx);
  end; !case
end; !AddWkDay

procedure AddMonth (
	ref D: DateRec;
	var Date: DateLineType;
	var Ndx: integer );

begin
!writeln('Date so far: ', Date);
!writeln('Ndx: ', Ndx);
!writeln(D.Month);
  case D.Month of
    0: CatString('January    ', 8, Date, Ndx);
    1: CatString('February   ', 9, Date, Ndx);
    2: CatString('March      ', 6, Date, Ndx);
    3: CatString('April      ', 6, Date, Ndx);
    4: CatString('May        ', 4, Date, Ndx);
    5: CatString('June       ', 5, Date, Ndx);
    6: CatString('July       ', 5, Date, Ndx);
    7: CatString('August     ', 7, Date, Ndx);
    8: CatString('September  ', 10, Date, Ndx);
    9: CatString('October    ', 8, Date, Ndx);
    10: CatString('November   ', 9, Date, Ndx);
    11: CatString('December   ', 9, Date, Ndx);
  end; !case
!writeln('Now date is: ', Date);
!writeln('Ndx: ', Ndx);
end; !AddMonth

procedure AddTime (
	ref D: DateRec;
	var Date: DateLineType;
	var Ndx: integer );

begin !AddTime
  if D.Hour < 10 then begin
      Date[Ndx] := '0';
      Ndx := Ndx + 1;
    end;
  CatNum(D.Hour, Date, Ndx);
  Date[Ndx] := ':';
  Ndx := Ndx + 1;
  if D.Min < 10 then begin
      Date[Ndx] := '0';
      Ndx := Ndx + 1;
    end;
  CatNum(D.Min, Date, Ndx);
  Date[Ndx] := ':';
  Ndx := Ndx + 1;
  if D.Sec < 10 then begin
      Date[Ndx] := '0';
      Ndx := Ndx + 1;
    end;
  CatNum(D.Sec, Date, Ndx);
end; !AddTime

procedure FormatDate (
	ref D: DateRec;
	var Date: DateLineType
	);

var Ndx: integer; Year: integer;

begin !FormatDate
  Ndx := 1;
  AddWkDay(D, Date, Ndx);
  AddMonth(D, Date, Ndx);
  CatNum(D.MDay, Date, Ndx);
  CatString(',          ', 2, Date, Ndx);
  Year := D.Year + Century;
  CatNum(Year, Date, Ndx);
  CatString('           ', 3, Date, Ndx);
  AddTime(D, Date, Ndx);
  Date[Ndx] := chr(0);
end; !FormatDate

!*****************************************************************************
! The following types and routines are used for reading, scanning and writing
! lines of text.
!*****************************************************************************

WdRange = 1..9;		!Index type for word

WdType = packed array [WdRange] of char;

WordType = record
	Length: WdRange;
	Wd: WdType;
end;


! Types of tokens the program recognizes

TType = (CommentT, EndT, RParenT, LParenT, ColonT, NumT, WordT);

TokenType = record
case T: TType of
	WordT: (Word: WordType);	!Alphanumeric
	NumT: (Num: integer);		!Numerical argument
end;


LineRange = 1..256;	!Index type for lines

! The following defines a line of text, its current length, and the first
! unscanned character in the line

LineType = record
	Length: 1 .. LineRange.High;	!real length of line
	Idx: integer;			!index of first unread character
	Text: packed array [LineRange] of char;
end;

!************************************************************************
! The following routines read lines from input, write a line to output,
! (including formatting the date field for easier reading), and scan
! tokens from the line.

!************************************************************************
! I/O routines

procedure ReadLine (var F: text; var Line: LineType); !Read a single input line.

var i : integer;

begin !ReadLine
! writeln('In ReadLine');
! writeln('LineRange.High is:  ', LineRange.High);
  i := 0;
  while ((i < LineRange.High) and (not eof(F)) and (not eoln(F))) do begin
      i := i + 1;
      read(F, Line.Text[i]);
! writeln('Just read:  ', Line.Text[i]);
    end;
  Line.Length := i;
  if eoln(F) then readln(F);
end; !ReadLine


! Write one 'word' i.e., all characters not blank separated, to standard
! output.  Used to write the date which is not fixed column format.

procedure WriteWord (
	ref DateLn: DateLineType;
	var CurPos: integer );

begin
  while (CurPos <= DateLineRange.High) and (DateLn[CurPos] <> ' ')
    and (DateLn[CurPos] <> chr(0)) do begin
      write(DateLn[CurPos]); CurPos := CurPos + 1
    end;
  CurPos := CurPos + 1;		!Advance past blank or null
end;

procedure WriteLine (Pos: integer; ref Line: LineType);

var I: integer;

begin !WriteLine
  for I := Pos to Line.Length do write(Line.Text[I]);
  writeln;
end; !WriteLine

! Write line with date
procedure WriteDateLine(ref CD: DateRec; ref Line: LineType);

var
  I: integer;
  Date: DateLineType;
  DPos, LPos: integer;

begin !WriteDateLine
  FormatDate(CD, Date);
  DPos := 1;
  for I := 1 to 4 do begin	!Write the matching date, end with ';'
      WriteWord( Date, DPos); if I = 4 then write(';') else write(' ');
    end;
  LPos := 1;
  while (LPos <= Line.Length) and (Line.Text[LPos] <> ';') do
      LPos := LPos + 1;
  LPos := LPos + 1; !pass semicolon
  WriteLine(LPos, Line);
end; !WriteDateLine

!***************************************************************************
! Scan routines.  All scan routines reset the Line.Idx field to next unread
! character.  If a calling routine wishes to restart, or back up this must
! be reset.

Digits = ['0'..'9'];

Letters = ['a'..'z', 'A'..'Z'];

LettersOrDigits = ['a'..'z', 'A'..'Z', '0'..'9'];

ScanChars = ['#', ';', ':', '0'..'9', 'a'..'z', 'A'..'Z'];

! Passes uninteresting characters in the input line, beginning at L.Idx.
! Returns the new current line position in L.Idx.

procedure SkipJunk ( var L: LineType );		!The input line

begin !SkipJunk
  while (L.Idx <= L.Length) and not (L.Text[L.Idx] in ScanChars) do
      L.Idx := L.Idx + 1;
end; !SkipJunk

! This routine scans off an identifier (1st character alpha, rest alphanumeric),
! to be checked against the function identifier table.

procedure ExtractWord (
	var L: LineType;	!The input line
	var Word: WordType);	!The word returned
	
var
  Ndx: integer;
  Ch: char;

begin
  Ndx := 0;
  if L.Idx <= L.Length then Ch := L.Text[L.Idx];
  if Ch in Letters then begin
      while (Ndx < WdRange.High) and (L.Idx <= L.Length) and
        (Ch in LettersOrDigits) do begin
          Ndx := Ndx + 1;
          if (ord(Ch) >= ord('A')) and (ord(Ch) <= ord('Z'))
          then Word.Wd[Ndx] := chr( ord(Ch) + (ord('a') - ord('A')) )
          else Word.Wd[Ndx] := Ch;
          L.Idx := L.Idx + 1;
          if (L.Idx <= L.Length) then Ch := L.Text[L.Idx];
        end; !while loop
      Word.Length := Ndx;
    end; !if Letters
end; !ExtractWord

procedure ExtractNum (	!Convert digits from the input line to an integer
	var L: LineType;		!The input line
	var N: integer);		!The integer returned
	
var Ch: char;

begin !ExtractNum
  N := 0;
  if (L.Idx <= L.Length) then Ch := L.Text[L.Idx];
  while (L.Idx <= L.Length) and (Ch in Digits) do begin
      N := (N*10) + (ord(Ch) - ord('0'));
      L.Idx := L.Idx + 1;
      if (L.Idx <= L.Length) then Ch := L.Text[L.Idx];
    end; !while loop
end; !ExtractNum

! Scan one token off of the input line.  Advance the line pointer.

procedure ExtractToken (
	var Line: LineType;
	var Token: TokenType );

begin !ExtractToken
  SkipJunk(Line);
  if (Line.Idx > Line.Length) then
      Token.T := EndT
  else
    case Line.Text[Line.Idx] of
      '#': Token.T := CommentT;
      ';': Token.T := EndT;
      '0'..'9': begin
	   Token.T := NumT;
	   ExtractNum(Line, Token.Num)
         end;
      ':': begin
	  Token.T := ColonT;
	  Line.Idx := Line.Idx + 1;
        end;
      'A'..'Z', 'a'..'z': begin
          Token.T := WordT;
          ExtractWord(Line, Token.Word)
        end;
      otherwise writeln('ERROR in ExtractToken, unknown token'); !bug
    end; !case
end; !ExtractToken

!***************************************************************************
! Types and routines which define, initialize, and search a name table.

FRtnType = (FNullLine, FOk, FFail, FError, FAmbiguous, FNotFound);!Return codes

FType = (DayNm, MonthNm, UserNm);  !Function types: month, day, and user names

EntryType = record	!Function name table entry definition.
	Nm: WdType;	!The identifier name
	TypeNm: FType;	!Its type -- weekday, month, or user defined
	Value: integer;	!Its ordinal value
end;

TabRange = 1..19;	!Table size, potentially expandable

TabType = packed array [TabRange] of EntryType;    !The function name table.

var  FunctionTab: TabType;	!The function name table

!***************************************************************************
! Initialization routines

function InitFTabEntry (		!Initialize one table entry.
	FNm: WdType;
	FTypeNm: FType;
	FValue: integer ) Entry: EntryType;

begin !InitFTabEntry
  Entry.Nm := FNm; Entry.TypeNm := FTypeNm; Entry.Value := FValue;
end; !InitFTabEntry

procedure InitFTab (var FTab: TabType);	!Initialize the function name table.

begin !InitFTab
  FTab[1] := InitFTabEntry('april    ', MonthNm, 3);
  FTab[2] := InitFTabEntry('august   ', MonthNm, 7);
  FTab[3] := InitFTabEntry('december ', MonthNm, 11);
  FTab[4] := InitFTabEntry('february ', MonthNm, 1);
  FTab[5] := InitFTabEntry('friday   ', DayNm, 5);
  FTab[6] := InitFTabEntry('january  ', MonthNm, 0);
  FTab[7] := InitFTabEntry('july     ', MonthNm, 6);
  FTab[8] := InitFTabEntry('june     ', MonthNm, 5);
  FTab[9] := InitFTabEntry('march    ', MonthNm, 2);
  FTab[10] := InitFTabEntry('may      ', MonthNm, 4);
  FTab[11] := InitFTabEntry('monday   ', DayNm, 1);
  FTab[12] := InitFTabEntry('november ', MonthNm, 10);
  FTab[13] := InitFTabEntry('october  ', MonthNm, 9);
  FTab[14] := InitFTabEntry('saturday ', DayNm, 6);
  FTab[15] := InitFTabEntry('september', MonthNm, 8);
  FTab[16] := InitFTabEntry('sunday   ', DayNm, 0);
  FTab[17] := InitFTabEntry('thursday ', DayNm, 4);
  FTab[18] := InitFTabEntry('tuesday  ', DayNm, 2);
  FTab[19] := InitFTabEntry('wednesday', DayNm, 3);
end; !InitFTab

!***************************************************************************
! Table search routines.

function MatchName (	!Match one name against a single table entry.
	ref Mn: WordType;
	ref TabEntry: WdType ) : boolean;

var Ndx: WdRange;

begin
! writeln('in MATCH NAME ');
  Ndx := 1;
  while (Ndx < Mn.Length) and (Mn.Wd[Ndx] = TabEntry[Ndx]) do Ndx := Ndx + 1;
  MatchName := (Mn.Wd[Ndx] = TabEntry[Ndx]);
end;

! Search the name table for a matching entry.  Check for ambiguity.  Return
! the table index for the correct entry, and an error code.

function SearchFTab (
	ref Mn: WordType;
	ref FTab: TabType;
	var TabNdx: integer )FRtn: FRtnType;

var
  Ndx: integer;
  MatchFlag: boolean;
  TmpNdx: integer;

begin !SearchFTab
! writeln('in SEARCH F TAB ');
  Ndx := 1;
  TabNdx := 1;
  while (Mn.Wd[Ndx] > FTab[TabNdx].Nm[Ndx]) and (TabNdx < TabRange.High) do
      TabNdx := TabNdx + 1;
! writeln('At table entry: ', FTab[TabNdx].Nm);
  MatchFlag := false;
! writeln('SEARCH F TAB start while loop');
  while (TabNdx <= TabRange.High) and (Mn.Wd[Ndx] = FTab[TabNdx].Nm[Ndx])
    and not MatchFlag do begin
! writeln('SEARCH F TAB Calling MatchName');
      if MatchName(Mn, FTab[TabNdx].Nm) then begin
! writeln('SEARCH F TAB matching entry is ', FTab[TabNdx].Nm);
          MatchFlag := true;
	  TmpNdx := TabNdx + 1;
	  if (TmpNdx <= TabRange.High)
	    and MatchName(Mn, FTab[TmpNdx].Nm) then begin
	      FRtn := FAmbiguous;
! writeln('SEARCH F TAB:  function name AMBIGUOUS');
            end {ambiguous}
	  else FRtn := FOk
        end !if matchname()
      else TabNdx := TabNdx + 1;
    end; !while
! writeln('SEARCH F TAB end of while loop');
  if not MatchFlag then begin
      FRtn := FNotFound;
! writeln('in SEARCH F TAB -- error condition cannot find function name');
    end; {not MatchFlag}
! writeln('end of SEARCH F TAB');
end; !SearchFTab

!****************************************************************************
! The following types and routines are used to check a value against a
! numeric range.
!****************************************************************************

! This type is used to evaluate numerical arguments, or week or month day
! ordinal values.

RangeType = record
	Low: integer;
	High: integer
end;

function MakeRange ( Low, High: integer ) Range: RangeType;

begin !MakeRange
  Range.Low := Low; Range.High := High;
end; !MakeRange

function EvaluateRange (
	Range: RangeType;
	Value: integer ) Rtn : boolean;

begin !EvaluateRange
  Rtn := (Value >= Range.Low) and (Value <= Range.High);
!  if Rtn then
!writeln('EVALUATE range TRUE for: ', Value, Range.Low, Range.High)
!  else
!writeln('EVALUATE range is FALSE for: ', Value, Range.Low, Range.High);
end; !EvaluateRange
 
! This routine checks a numeric range against a specific value.  If any value
! within the range matches Value true is returned, otherwise CheckRange
! returns false.  If the range is specified high to low, i.e., Range.Low is
! greater than Range.High, rather than low to high,  it is evaluated as
! 2 ranges: from Range.Low to Bounds.High, and from Bounds.Low to Range.High.

! For now, it is assumed that the range is mismatched if either the high
! or low end of the range is outside of the Bounds range.

function CheckRange (
	Range: RangeType;
	Value: integer;
	Bounds: RangeType) Rtn : boolean;

var TRange, T2Range: RangeType;

begin !CheckRange
! writeln('Range is: ', Range.Low, ' to ', Range.High);
! writeln('Bounds are: ', Bounds.Low, ' to ', Bounds.High);
  with R = Range do begin
      !Check that the range is within the range bounds
      if ( R.High > Bounds.High ) or ( R.Low > Bounds.High )
	or ( R.High < Bounds.Low ) or ( R.Low < Bounds.Low )
      then begin
	  Rtn := false;	!we are out of the range bounds
! writeln('Out of bounds');
        end
      else begin	!do range check
          if ( R.High >= R.Low ) then !range is low-high
	      Rtn := EvaluateRange(Range, Value)
          else begin !range is high-low
	      TRange := MakeRange(R.Low, Bounds.High);
	      T2Range := MakeRange(Bounds.Low, R.High);
	      Rtn := EvaluateRange(TRange, Value)
	      or EvaluateRange(T2Range, Value);
	    end !else range is high-low
	end; !else do range check
    end; !with
! if Rtn then writeln('CHECKRANGE RETURNING TRUE');
end; !CheckRange

!***************************************************************************
! The following routines evaluate different kinds of functions based on
! the FType in the function table entry (month names, weekday names, and
! user defined names).
!***************************************************************************

function DoNameRange (
	ref FTab: TabType;	!Function name table
	TabNdx: integer;	!Index into the name table
	FNameType: FType;	!Type of the name range
	Value: integer;		!Value to check against the range
	Bounds: RangeType;	!The range boundary for this type
	var Line: LineType;	!Current Line
	var Token: TokenType	!Current token
	) FRtn : FRtnType;

var Range: RangeType;

begin ! DoNameRange
  FRtn := FOk;
  Range.Low := FTab[TabNdx].Value;
! writeln('DO NAME RANGE: Matching entry is: ', FTab[TabNdx].Nm);
! writeln('ordinal value is: ', FTab[TabNdx].Value);
  ExtractToken(Line, Token);
  if Token.T <> ColonT then Range.High := Range.Low
  else begin	!range 
      ExtractToken(Line, Token);
      if Token.T <> WordT then begin !error
	  FRtn := FError;
! writeln('In DO NAME RANGE:  error wrong token type!!');
	end
      else begin !assign range value
! writeln('DO NAME RANGE: about to search FTab');
	  FRtn := SearchFTab(Token.Word, FTab, TabNdx);
 ! writeln('DO NAME RANGE: Matching entry is: ', FTab[TabNdx].Nm);
 ! writeln('ordinal value is: ', FTab[TabNdx].Value);
	  if (FRtn = FOk) and (FTab[TabNdx].TypeNm <> FNameType) then begin
	      FRtn := FError;	!Range values must be of the same type
! writeln('In DO NAME RANGE:  error wrong name type !!!');
	    end;
	  if (FRtn = FOk) then begin
	      Range.High := FTab[TabNdx].Value;
	      ExtractToken(Line, Token)
	    end;
        end; !else, assign range value
    end; !range
 ! if (FRtn = FOk) then writeln('In DoNameRange about to check: ', Range.Low,
 ! '-', Range.High, 'for value: ', Value);
  if (FRtn = FOk) then
      if CheckRange(Range, Value, Bounds) then FRtn := FOk else FRtn := FFail;
end; ! DoNameRange

! This table indicates whether or not there is a currently active value for
! the day of the month, the month, and the year, as the date is evaluated.
! It resolves ambiguities, for example, it ensures that 1-1 doesn't match all
! of January.  It also ensures that if a month name was not specified, the
! numeric value of the month is matched before the numeric value of the day of
! the month.  This guarantees that 6-4 matches june 4, and 4-6 matches april 6.

FlagRecType = packed record
	MonthSeen: boolean;
	DaySeen: boolean;
	YearSeen: boolean;
end;

! This routine processes function identifiers.  It is not a general purpose
! parser.  It searches the function name table for specific words it understands
! the function of.  If one is found, the appropriate action is taken.

function DoFunction (
	ref FTab: TabType;
	ref Name: WordType;
	ref CD: DateRec;
	var Flags: FlagRecType;
	var Line: LineType;
	var NextToken: TokenType;
	)FRtn: FRtnType;

var
  TabEntry: EntryType;
  TabNdx: integer;
  TRange: RangeType;

begin !DoFunction
! writeln(' in DOFUNCTION: about to search F Tab ');
  FRtn := SearchFTab(Name, FTab, TabNdx);
! writeln('DO FUNCTION: returned from search');
  if FRtn = FOk then
    case FTab[TabNdx].TypeNm of
      MonthNm : begin
	  TRange := MakeRange(0,11);
! writeln('DO FUNCTION: about to check range for month');
	  FRtn := DoNameRange(FTab, TabNdx, MonthNm, CD.Month, TRange,
	    Line, NextToken);
! writeln('just checked month name.');
	  if (FRtn = FOk) then Flags.MonthSeen := true;
        end;
      DayNm :  begin
	  TRange := MakeRange(0,6);
! writeln('DO FUNCTION: about to check range for day of the week.');
          FRtn := DoNameRange(FTab, TabNdx, DayNm, CD.WkDay, TRange,
	    Line, NextToken);
! writeln('just checked week day name.');
        end;
      UserNm : begin
	  ExtractToken(Line, NextToken);
        end;
    end;
end; !DoFunction

!***************************************************************************
! The following routines handle numeric fields.  There is a routine to read
! and set the field value and a routine to check whether or not there is a
! valid match for the field in the date we are currently checking against.
!***************************************************************************

! This routine checks a numeric range against the current month (if it has not
! already been found), or the current day of the month (if it hasn't already
! been found), or the current year (if it hasn't been found).
! When a matching value is found the corresponding flag field (MonthSeen,
! DaySeen or YearSeen) is set to true, and the routine exits.

function CheckNum (
	N: RangeType;
	var Flags: FlagRecType;
	ref CD: DateRec ) FRtn: FRtnType;

var Val: integer; TRange, T2Range: RangeType;

begin !CheckNum
! writeln('In CheckNum range is: ', N.low, ' to ', N.high);
  FRtn := FFail;
  with F = Flags do begin
      if F.MonthSeen and F.YearSeen and F.DaySeen then begin
! writeln('error, too many numbers');
	  FRtn := FError;
	end	!Error, too many numbers
      else begin !Check values
	  if not F.MonthSeen then begin
	      Val := CD.Month + 1;
	      TRange := MakeRange(1,12);
              if CheckRange(N, Val, TRange) then begin	!Found the month
! writeln('Found the month');
	          FRtn := FOk; F.MonthSeen := true;
		end; !if CheckRange ()
            end; !if not F.MonthSeen
          if (FRtn = FFail) and not F.DaySeen then begin
	      TRange := MakeRange(1,31);
              if CheckRange(N, CD.MDay, TRange) then begin !Got day of the month
! writeln('Found the day of the month');
	          FRtn := FOk; F.DaySeen := true;
		end; !if CheckRange()
            end; !if FFail and not DaySeen
          if (FRtn = FFail) and not F.YearSeen then begin
	      Val := CD.Year + Century;
	      T2Range := MakeRange(1900, 2500);
	      TRange := MakeRange(0,100);
              if CheckRange(N, CD.Year, TRange) or CheckRange(N, Val, T2Range)
	      then begin	!Found the year
! writeln('Found the year');
	          FRtn := FOk; F.YearSeen := true;
	        end;
            end; !if FFail and not YearSeen
      end; !else check value
    end; !with
end; !CheckNum

! This routine is called when a numeric value has been scanned.  It determines
! if the user specified a single value or a range and sets the corresponding
! range value (a single value is treated as a range of n:n).

function SetRange (
	var NumRange: RangeType;
	var Line: LineType;
	var NextToken: TokenType;
	) FRtn: FRtnType;

begin !SetRange
  FRtn := FOk;
  NumRange.Low := NextToken.Num;
  ExtractToken(Line, NextToken);
  if NextToken.T = ColonT then begin	!range
! writeln('Got colon sym.');
      ExtractToken(Line, NextToken);
      if NextToken.T = NumT then begin
! writeln('set range high field.');
	  NumRange.High := NextToken.Num;
	  ExtractToken(Line, NextToken)
	end !if NumT
      else begin !range tokens must be the same type
	  FRtn := FError;
! writeln('in SET RANGE:  error wrong token type!!');
	end !else
    end !if range
  else NumRange.High := NumRange.Low;
! writeln('Range value is: ', NumRange.Low, ' to ', NumRange.High);
end; !SetRange

! The following routine determines if the numbers scanned are valid for the
! current date.  No field is matched twice.

NumArrayType = array [1..3] of RangeType;

function DoNumbers (
	ref Numbers: NumArrayType;		!Array of range values
	NIdx: integer;			!How many numbers were scanned
	Flags: FlagRecType;		!What fields are filled
	ref CD: DateRec ) FRtn: FRtnType;	!The current date

begin !DoNumbers
  FRtn := FOk;
  if (NIdx >= 1) then begin
! writeln('Checking Numbers[1]:  ', Numbers[1].Low, ' to ', Numbers[1].High);
      FRtn := CheckNum(Numbers[1], Flags, CD);
!On a single number we want to match the day of the month.
!We allow the year if it matches unambiguously.
      if (NIdx = 1) and (FRtn = FOk) and not Flags.DaySeen then begin
	  if (Numbers[1].Low <= 31) and (Numbers[1].High <= 31) then begin
	  ! We are in day of month range.  Otherwise, we must have had valid,
	  ! unambiguous year since month range is a subset of day range 
	      FRtn:= CheckNum(Numbers[1], Flags, CD); !Check if month=month day
	      if not Flags.DaySeen then FRtn := FFail;
            end; !if in day of the month range
        end; !if single number and not DaySeen
    end; !if NIdx >= 1
  if (FRtn = FOk) then !look for valid second number
      if (Flags.MonthSeen or Flags.YearSeen) and (NIdx >= 2) then begin
! writeln('Checking Numbers[2]:  ', Numbers[2].Low, ' to ', Numbers[2].High);
          FRtn := CheckNum(Numbers[2], Flags, CD)
        end !if valid second number
      else if (NIdx >= 2) then FRtn := FFail; !invalid second number
  if (FRtn = FOk) then !look for valid third number
      if Flags.MonthSeen and (NIdx = 3) then begin
! writeln('Checking Numbers[3]:  ', Numbers[3].Low, ' to ', Numbers[3].High);
          FRtn := CheckNum(Numbers[3], Flags, CD)
        end !valid third number
      else if (NIdx = 3) then FRtn := FFail; !invalid third number
end; !DoNumbers

!****************************************************************************
! The following routine determines whether the date in the current input line
! matches the 'current date'.  Function names are evaluated immediately.
! Numerical fields are only checked after all function names have been pro-
! cessed.  At any time the date proves invalid we return to ProcessLine.
! Evaluating numerical fields last is useful if there is overlap in the month
! and the day of the month ranges.  If a month name was found then none of the
! numeric fields may be the month.  If a month name has not been found the
! month value must be found before the day of the month.

function MatchDate (
	var Line: LineType;
	ref CD: DateRec;)FRtn : FRtnType;

var
  NextToken: TokenType;
  Numbers: NumArrayType;
  NIdx: integer;
  Flags: FlagRecType;

begin !MatchDate
  !Init flags to false
  Flags.MonthSeen := false; Flags.DaySeen:= false; Flags.YearSeen := false;
  Line.Idx := 1; !Set scan pointer to beginning of line
  NIdx := 0;
  FRtn := FOk;
  ExtractToken(Line, NextToken);
  if (NextToken.T = CommentT) then FRtn := FNullLine
  else begin
      while (NextToken.T <> EndT) and (FRtn = FOk) do begin
! writeln('Curpos is: ', Line.Idx, 'Line length is: ', Line.Length);
! writeln('NextToken.T = ', NextToken.T);
          case NextToken.T of
	    CommentT: begin
		NextToken.T := EndT;
		Line.Length := Line.Idx - 1;
              end; !case CommentT
            NumT: begin
! writeln('NextToken.Num = ',NextToken.Num);
	        if NIdx < 3 then begin
		    NIdx := NIdx + 1;
		    FRtn := SetRange(Numbers[NIdx], Line, NextToken);
! writeln('Range is:  ', Numbers[NIdx].Low, ' to ', Numbers[NIdx].High);
! writeln('NIdx is:  ', NIdx);
	          end
	        else begin
		    FRtn := FError;
! writeln('in MATCH DATE: error read too many numbers!!!!');
	          end; {else}
              end; !case NumT
            WordT: begin
! writeln('NextToken.Word = ', NextToken.Word.Wd: NextToken.Word.Length);
! writeln('calling DoFunction...');
	        FRtn := DoFunction(FunctionTab, NextToken.Word, CD, Flags,
	          Line, NextToken);
              end; !case WordT
            otherwise begin 
	        FRtn := FError;
! writeln('MATCH DATE:  error unknown token type ?????????!!!!!!!!');
              end; {otherwise}
          end; {case Token.T}
      end; !while
      if (FRtn = FOk ) then FRtn := DoNumbers(Numbers, NIdx, Flags, CD);
  end; !else
end; !MatchDate

!****************************************************************************
! These routines process one input line.  The line is checked for the current
! date.  If there is no error and no match the current date is advanced one day.
! The line is checked again.  'Lookahead' is extended to Monday for weekends.

function CheckNext (	!Do one day 'lookahead' for a given date.
	var Line: LineType;
	var CurDate: DateRec;
	var LTime: integer;
	) FRtn: FRtnType;

begin !CheckNext
  LTime := LTime + 86400;
  BuildCurDate(LTime, CurDate);
  FRtn := MatchDate(Line, CurDate);
end; !CheckNext

function Blank (var Line: LineType) : boolean;

begin !Blank
  Line.Idx := 1;
  SkipJunk(Line);
  Blank := (Line.Idx > Line.Length);
end; !Blank

! This routine checks an input line for a matching date, does lookahead if
! necessary, prints the line and an error message if an error occurs, and
! prints the output line with a pretty date if there is a match.

procedure ProcessLine (
	var Line: LineType;
	ref CurDate: DateRec;
	LTime: integer);

var
  FRtn: FRtnType;
  CD: DateRec;
  LT: integer;
  I: integer;

begin !ProcessLine
  CD := CurDate; LT := LTime;	!Copy parameters to local temporaries
  if not Blank(Line) then begin
      FRtn := MatchDate(Line, CD);
      if (FRtn = FFail) then begin !do lookahead
          FRtn := CheckNext(Line, CD, LT);
          if (FRtn = FFail) and ((CD.WkDay = 6) or (CD.WkDay = 0))
          then FRtn := CheckNext(Line, CD, LT);
          if (FRtn = FFail) and (CD.WkDay = 0)
          then FRtn := CheckNext(Line, CD, LT);
        end;
      case FRtn of
        FError, FNotFound, FAmbiguous: begin
	    WriteLine(1, Line);
	    for I := 1 to Line.Idx - 1 do write(' ');
	    writeln('^');
	    writeln('*** I don''t understand this.');
          end;
        FOk: WriteDateLine(CD, Line);
        FNullLine, FFail: ;
      end; {Case}
  end; !if blank
end; !ProcessLine

!******************************************************************************
! Main program

var
  LTime: integer;	!Local time in seconds, reset for look ahead
  Line: LineType;	!The current line of data from the input file
  Infile: text;		!The reminder file



begin
! writeln('starting');
! writeln('initTable');
  InitFTab(FunctionTab);
! writeln('get time');
  GetTime(LTime);
! writeln('get date');
  BuildCurDate(LTime, CurDate);
  reset(Infile, 'reminder');
  while not eof(Infile)
  do
    begin
      ReadLine(Infile, Line);
! writeln('processing file line');
      ProcessLine(Line, CurDate, LTime);
    end;
! writeln('done');
end.
