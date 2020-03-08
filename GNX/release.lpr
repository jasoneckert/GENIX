



















                       NATIONAL SEMICONDUCTOR CORPORATION


                                  GENIX SOURCE

                                 RELEASE LETTER









































                                  GENIX SOURCE


_R_e_l_e_a_s_e _P_a_c_k_a_g_e _C_o_n_t_e_n_t_s



 Part Number                                Description
091010137-001      Customer Support Package consisting of:
                   GENIX Programmer's Manual Volume 1
                   GENIX Programmer's Manual Volume 2
                   SYS16 Systems Manual
440010137-001      A 2400ft, 9-track, 1600 BPI, magnetic tape, having the
                   label, "GENIX1.0AI" and as per the dump and tar listings
                   in the "File Listings" section.
440010137-002      A 2400ft, 9-track, 1600 BPI, magnetic tape, having the
                   label, "GENIX1.0AII" and as per the dump and tar listings
                   in the "File Listings" section.
440010137-003      A 2400ft, 9-track, 1600 BPI, tar-format magnetic tape,
                   having the label, "GENIX1.0AIII" and as per the tar listing
                   in the "File Listings" section.
925010137-001      GENIX Cross-Support Software Package























     Note: GENIX and SYS16 are trademarks of National Semiconductor Corporation.

















                                 433010137-001                      Page 2 of 93


                                  GENIX SOURCE


     _S_o_f_t_w_a_r_e _P_r_o_d_u_c_t _D_e_s_c_r_i_p_t_i_o_n



        Name:       GENIX Source (part number, 970010137-001 Rev A)

      Version:       1.0


     GENIX Source consists of:


(i)  All the source files and support files needed for the  re-creation  of  the
     GENIX Operating System for the SYS16 development system.


(ii) A representation, on tape, of the binary version  of  the  GENIX  Operating
     System as would be supplied, on cartridge, with a SYS16 development system.


(iii)
     The GENIX Cross-Support Software  package  -  included  to  facilitate  the
     rebuilding  of  this system and to provide an advanced software development
     environment for the NS16000 programmer.


(iv) A Customer Support Package, consisting of  complementary  documentation  to
     the  GENIX/SYS16  development  environment (this documentation is also pro-
     vided on tape).















     Note: The revision number (Rev A) above corresponds to a particular
           GENIX Source release version (version 1.0).














                                 433010137-001                      Page 3 of 93


                                  GENIX SOURCE


     _C_o_m_p_a_t_i_b_i_l_i_t_y _I_s_s_u_e_s



1.   GENIX Source was developed under UNIX (Berkeley 4.1 bsd) running on  a  VAX
     11/780  computer. All C code in GENIX Source is compatible with the cc com-
     piler in Berkeley 4.1 bsd.  National  Semiconductor  does  not  assure,  or
     guarantee, in any way, compatibility of GENIX Source with any other release
     version or port, of UNIX, or of cc.


2.   Users of GENIX Source modify source code at their own risk.  National  Sem-
     iconductor  does not support any GENIX Source component modified by a user,
     nor any component that interfaces with a modified component.


3.   GENIX Source is compatibile with the following revisions of  National  Sem-
     iconductor chips:

      NS 16032    -    Rev G
      NS 16082    -    Rev H
      NS 16081    -    Rev C1
      NS 16201    -    Rev C
      NS 16202    -    Rev D


4.   GENIX Source contains the source code that is used to  generate  the  GENIX
     Operating  System that runs on the National Semiconductor SYS16 development
     system. GENIX Source is compatible with this system, but compatibility with
     other hardware environments cannot be assured.















     Note: UNIX is a trademark of Bell Laboratories.
           VAX is a trademark of Digital Equipment Corporation.













                                 433010137-001                      Page 4 of 93


                                  GENIX SOURCE


     _I_n_s_t_a_l_l_a_t_i_o_n _P_r_o_c_e_d_u_r_e



     (_P_l_e_a_s_e _r_e_a_d _t_h_e _w_h_o_l_e _o_f _t_h_i_s _s_e_c_t_i_o_n _t_h_o_r_o_u_g_h_l_y _b_e_f_o_r_e _p_r_o_c_e_e_d_i_n_g -  _s_e_c_-
     _t_i_o_n  (_d) _e_x_p_l_a_i_n_s _h_o_w _t_o _i_n_s_t_a_l_l _G_E_N_I_X _S_o_u_r_c_e _f_o_r _e_a_s_y _r_e_c_o_m_p_i_l_a_t_i_o_n _u_s_i_n_g
     _t_h_e _m_a_k_e_f_i_l_e_s _s_u_p_p_l_i_e_d.)


     Login into superuser - the commands below assume you are using the  default
     tape drive '0' and that you have at least 62598 blocks of disk space avail-
     able.


(a)  Mount the tape labeled, _G_E_N_I_X_1._0_A_I, _W_I_T_H_O_U_T _A _T_A_P_E _W_R_I_T_E-_R_I_N_G and with  the
     tape  density of the drive set at _1_6_0_0 _B_P_I.  Using the following procedure,
     restore the dump and tape archives to the directory of your  choice.  (_N_o_t_e
     _t_h_a_t  _t_h_e  _m_a_k_e_f_i_l_e_s _s_u_p_p_l_i_e_d _p_r_e_s_u_m_e _t_h_i_s _d_i_r_e_c_t_o_r_y _t_o _b_e _n_a_m_e_d /_m_e_s_a, _f_o_r
     (_i) & (_i_v) _b_e_l_o_w, _a_n_d /_m_e_s_a _f_o_r (_i_i) & (_i_i_i) - _s_e_e _s_e_c_t_i_o_n (_d) _f_o_r  _f_u_r_t_h_e_r
     _i_n_f_o_r_m_a_t_i_o_n.)


       (i) The first section of the tape is a  dump  of  the  _d_e_v  directory  as
     represented  in  the SYS16 development system and may be restored using the
     following commands:

     % _m_k_f_s  '_f_i_l_e_s_y_s_t_e_m-_n_a_m_e'  < _c_r >
     % _r_e_s_t_o_r  _r  '_f_i_l_e_s_y_s_t_e_m-_n_a_m_e'  < _c_r >


      (ii) The first tape archive, contains the source directories, saved  under
     _s_r_c/ and may be restored with the following commands:

     % _m_t  _f_s_f  < _c_r >
     % _c_d  '_d_i_r_e_c_t_o_r_y _o_f _y_o_u_r _c_h_o_i_c_e'  < _c_r >
     % _t_a_r  _x_v  < _c_r >


     (iii) The second tape archive, contains the _u_s_r/_i_n_c_l_u_d_e files,  and,  other
     usr  directories  (some empty) presumed to be present in the "installation"
     phase of the makefile process described in section  (d)  below.   They  are
     saved under _u_s_r/ and may be restored with the following commands:

     % _m_t  _f_s_f  _2  < _c_r >
     % _c_d  '_d_i_r_e_c_t_o_r_y _o_f _y_o_u_r _c_h_o_i_c_e'  < _c_r >
     % _t_a_r  _x_v  < _c_r >


      (iv) The third tape archive contains files and directories presumed to  be
     present  during  the  makefile  process  described in section (d) below. If
     required, you should restore, under a directory called /mesa, with the fol-
     lowing commands:

     % _m_t  _f_s_f  _3  < _c_r >
     % _c_d  /_m_e_s_a  < _c_r >
     % _t_a_r  _x_v  < _c_r >




                                 433010137-001                      Page 5 of 93


                                  GENIX SOURCE


       (v) The fourth  tape  archive,  contains  the  Pascal  compiler,  support
     libraries,  and other files presumed to be present during the makefile pro-
     cess of section (d). These files are saved under _N_S_C/ and must be installed
     under  /usr  to  have  the  pathname, /usr/NSC/ (as for GENIX Cross-Support
     Software) for the binaries to function correctly.  Use the  following  com-
     mands to restore:

     % _m_t  _f_s_f  _4  < _c_r >
     % _c_d  /_u_s_r  < _c_r >
     % _t_a_r  _x_v  < _c_r >


(b)  The second tape, labeled _G_E_N_I_X_1._0_A_I_I, has two sections: the  first  section
     contains  a  file-system  dump  of  all the binaries constituting the GENIX
     Operating System; with the second section containing a tape archive of  the
     _u_s_r  directory.   To  install, mount the tape _W_I_T_H_O_U_T _A _T_A_P_E _W_R_I_T_E-_R_I_N_G and
     with the tape density of the drive set at _1_6_0_0 _B_P_I.

     % _m_k_f_s  '_f_i_l_e_s_y_s_t_e_m-_n_a_m_e'  < _c_r >
     % _r_e_s_t_o_r  _r  '_f_i_l_e_s_y_s_t_e_m-_n_a_m_e'  < _c_r >
     % _m_t  _f_s_f  < _c_r >
     % _c_d  '_d_i_r_e_c_t_o_r_y _o_f _y_o_u_r _c_h_o_i_c_e'  < _c_r >
     % _t_a_r  _x_v  < _c_r


(c)  After dismounting the second tape, mount  the  tape  labeled  _G_E_N_I_X_1._0_A_I_I_I,
     _W_I_T_H_O_U_T  _A  _T_A_P_E  _W_R_I_T_E-_R_I_N_G  and with the tape density of the drive set at
     _1_6_0_0 _B_P_I, as before.  This tape has just one archive, containing the  docu-
     mentation,  saved  as directories, _m_a_n and _d_o_c.  Use the following commands
     to restore:

     % _c_d  '_d_i_r_e_c_t_o_r_y _o_f _y_o_u_r _c_h_o_i_c_e'  < _c_r >
     % _t_a_r  _x_v  < _c_r >


(d)  The makefiles supplied in the first archive of the first  tape,  presume  a
     specific configuration of source and destination directories: _s_r_c, from the
     first archive, and _u_s_r, from the second archive, are presumed to be located
     in  a  directory,  /v/mesaux  to form /v/mesaux/src, and /v/mesaux/usr; the
     third archive, as in (a)-(iii) above, is  presumed  to  be  loaded  into  a
     directory  called /mesa.  On re-making as per the procedure outlined below,
     the complete GENIX Operating System will consist of the binaries located in
     following directories:

          /v/mesaux/ _u_s_r
          /mesa/ _b_i_n
          /mesa/ _d_e_v
          /mesa/ _e_t_c
          /mesa/ _l_i_b
          /mesa/ _s_t_a_n_d
          /mesa/ _s_y_s
          /mesa/ _t_m_p


     Thus, if you were to use the makefiles provided as is,  you  would  do  the
     following:




                                 433010137-001                      Page 6 of 93


                                  GENIX SOURCE


       (i) First, install GENIX Cross-Support Software as per  the  instructions
     in the release letter enclosed with the package.


      (ii) Then, create the directories /v/mesaux and /mesa.


     (iii) Next, restore from the tape _G_E_N_I_X_1._0_A_I, _s_r_c, and _u_s_r into  /v/mesaux,
     and the third archive into /mesa as per the instructions in (a)-(iv).


      (iv) Then restore the files saved in the directory,  _N_S_C,  from  the  same
     tape _G_E_N_I_X_1._0_A_I, into /, as per the instructions in section (a)-(v).


      (iv) Finally, use the following commands to run the makefile process:

     % _c_d  /_v/_m_e_s_a_u_x/_s_r_c  < _c_r >
     % _m_a_k_e  _a_l_l_l_i_b  < _c_r >

     ("Make" all the libraries.)

     % _m_a_k_e  _i_n_s_l_i_b  < _c_r >

     ("Install" the libraries in /mesa/lib and /v/mesaux/usr/lib.)

     % _m_a_k_e  _a_l_l  < _c_r >

     (Initiate makefiles to rebuild binaries)

     % _m_a_k_e  _i_n_s_t_a_l_l  < _c_r >

     ("Install" the binaries in /mesa and /v/mesaux/usr.)

     % _c_d  /_v/_m_e_s_a_u_x/_s_r_c/_s_y_s/_S_Y_S_1_6  < _c_r >
     % _m_a_k_e  < _c_r >

     (This makefile makes _v_m_u_n_i_x which then should be installed into /mesa.)

     % _c_d  /_v/_m_e_s_a_u_x/_s_r_c/_s_y_s/_s_t_a_n_d  < _c_r >
     % _m_a_k_e  < _c_r >

     (This makefile makes the _s_t_a_n_d directory)

     % _m_a_k_e  _i_n_s_t_a_l_l  < _c_r >

     ("Install" in /mesa to form /mesa/stand.)













                                 433010137-001                      Page 7 of 93


                                  GENIX SOURCE


     _K_n_o_w_n _S_o_f_t_w_a_r_e _P_r_o_d_u_c_t _B_u_g_s

_A_S


A negative value is not allowed to appear in a '.FIELD' directive  which  has  a
field-length  value  of 32.  WORKAROUND: Use the '.DOUBLE' directive instead ---
the field-size is equivalent.


The Assembler ignores the amount of space allocated by any number  of  unlabeled
'.BLKi'  directives  within  a  procedure's Parameter and/or Return Value blocks
under certain circumstances.  This error causes any symbols defined in either of
these  blocks to be assigned incorrect Frame Pointer-Relative offsets. This hap-
pens when the last labeled '.BLKi' directive is followed by an unlabeled '.BLKi'
directive.  WORKAROUND: Label the last '.BLKi' directive in the Parameter and/or
Return Value block of the procedure in error.


The values in the range 64 to 65535, inclusive, and -65  to  -65536,  inclusive,
are  not  allowed  as  index values in explicitly-specified external operands in
which the offsets from the index have not been specified.   WORKAROUND:  Specify
an  offset  of  '0'  when  using  the  preceding  values as indices for external
operands.


In an extreme case, the Assembler goes into an infinite loop and,  at  the  same
time, generates an ever-expanding intermediate file in the /tmp directory.  This
happens when both operands of any instruction  are  forward-referenced  and  the
first  and (optionally) the second refer to a symbol which has a value of H'100.
Please note that the first operand must also make use of  a  displacement-length
specifier.   WORKAROUND: Put a 'NOP' instruction into your source file in such a
way that its presence forces the value of the forward-referenced  symbol  to  be
incremented past H'100.


The '.WIDTH' directive does not work.  WORKAROUND: None, except keeping all your
source  file lines within a 54-character limit --- especially if you have an 80-
column printer.


The Assembler does not issue an error message for any 'ASHi,' 'LSHi,' or  'ROTi'
instruction  which  has an out-of-range count operand.  WORKAROUND: None, except
making sure that:

     When i = 'B', the count  operand  does  not  exceed  the  range  -7  to  7,
     inclusive.

     When i = 'W', the count operand does  not  exceed  the  range  -15  to  15,
     inclusive.

     When i = 'D', the count operand does  not  exceed  the  range  -31  to  31,
     inclusive.


The Assembler does not issue  an  error  message  for  any  'EXTSi'  or  'INSSi'
instruction  which  has  an  out  of  range 'length' operand.  WORKAROUND: None,
except making sure that the value of the 'length' operand does  not  exceed  the


                                 433010137-001                      Page 8 of 93


                                  GENIX SOURCE


range 1 to 32, inclusive.


The Assembler does not issue an error message for a '.ALIGN' directive which has
a 'base' operand that causes an alignment to be made at a physical address which
exceeds the NS16032's addressing range of  16,777,215  memory  locations.   WOR-
KAROUND:  None,  except  making sure that you never make an alignment which goes
past the 16-Mbyte limit.


If a '.SUBTITLE' directive is the first line in a file and a '.TITLE'  directive
appears  before the first page break, only the subtitle will appear on the first
page of the listing file.  Normally, both pieces of text  would  appear  on  the
first page.  WORKAROUND: None.


If a '.SUBTITLE' directive is the first line encountered by the Assembler  after
a  page  break  in  a  file,  it  is  not used as the subtitle for the new page.
Instead, the subtitle of the previous page is  used.   WORKAROUND:  Specify  the
subtitle you want to appear on the next page before page break.


Not all instances of an undefined symbol in a source  file  are  flagged  by  an
error message.
WORKAROUND: Simply declare the symbol.


If an explicitly-specified external operand has an index value  which  has  been
qualified  by  a  displacement-length  specifier  and  this  value  exceeds  the
specifier's allowed range, an error message is not issued for the line in  which
the  error  occurs.  Instead, the Assembler will issue the error message when it
encounters the next instruction.  WORKAROUND: None, except making sure that:

     When the specifier is ':B', the index value does not exceed the  range  -64
     to 63, inclusive.

     When the specifier is ':W', the index value does not exceed the range -8192
     to 8191, inclusive.

     When the specifier is ':D', the index  value  does  not  exceed  the  range
     -16,777,215 to 16,777,215, inclusive.


The Assembler does not issue an error message for Register-Relative and  Memory-
Space displacements which clearly access data in memory locations outside of the
NS16032's address  space.   WORKAROUND:  None,  except  making  sure  that  your
Register-Relative and Memory-Space operands use displacements which do not cause
data outside of the 16-Mbyte address range to be accessed.


The Assembler does not issue an error message for a procedure's Parameter and/or
Return Value block which allocates more space than is available in the NS16032's
addressing range of 16,777,215 memory locations.  WORKAROUND: None, except  mak-
ing  sure  that the amount of storage allocated in either type of block does not
exceed the 16 Mbyte limit.





                                 433010137-001                      Page 9 of 93


                                  GENIX SOURCE


Simple floating-point constants with 19 or more digits before the decimal  point
that  also  have  exponent  values which result in the decimal point being left-
adjusted for most of these places are incorrectly translated  to  their  hexade-
cimal  representation.   WORKAROUND: Specify your floating-point constants in as
simple a form as possible.


When either operand of  an  instruction  is  a  symbol  having  the  PC-Relative
addressing  mode  with  an  offset  greater  than or equal to 256, the Assembler
issues  an  error  message   for  it  when  it  is  qualified  by  a   byte-long
displacement-length    specifier,    e.g.,   ':B'.    WORKAROUND:   Change   the
displacement-length specifier of the symbol to ':W' or ':D' or get  rid  of  the
specifier altogether.



When either operand of an instruction is a forward-referenced symbol  having
the  PC-Relative  addressing  mode,  displacements  for  any  subsequent PC-
Relative operands are not generated correctly under  certain  circumstances.
This  error will occur when the forward- referenced symbol is qualified by a
displacement-length specifier which forces the space used to store the  gen-
erated  displacement  to  be  more than the optimum amount necessary for the
given context.  WORKAROUND: Get rid of the displacement-length specifier.




The Assembler does not issue an error message for Data Generation Directives
which have an out of range value operand.  The specified value is checked to
see if it exceeds the double-word boundaries only.  If the value lies within
the  double-word  range  and  the amount of space necessary to represent the
value is compatible with the amount of space associated with  the  directive
specified,  then the space is intialized using the complete value.  However,
if the value passes the check but the amount of space necessary to represent
it  is  incompatible  with the amount of space associated with the directive
specified, then the necessary amount of the value's left-most bits are trun-
cated  so that its remaining bits fit into the directive's space.  When this
is done, the space is initialized using the  truncated  value.   WORKAROUND:
None, except making sure that the value specified for a particular directive
does not conflict with the directive's range of representable numbers.



_B_O_U_R_N_E _S_H_E_L_L


Long Bourne shell procedures may hang during execution. If a shell procedure
that hangs is broken down into shorter procedures and run individually, each
separate procedure may work. When a  procedure  that  hangs  is  executed  a
number  of times, that procedure may hang at seemingly arbitrarily different
points. A hanging procedure cannot be aborted via the "del"  key.  Control-z
does  not  stop  execution.  WORKAROUND: For long shell procedures use the C
shell instead.


_C_C




                                 433010137-001                     Page 10 of 93


                                  GENIX SOURCE


Initialization occurring in the same statement as a 'double' variable's declara-
tion  results  in  a FATAL compiler error if the initialization value used has a
positive exponent of 39 or greater.   WORKAROUND:  Do  not  initialize  'double'
variables at the same time you're declaring them.


When the '-c' flag is used, a  call  to  a  routine  which  initializes  certain
implicitly-declared external identifiers and explicitly-declared static identif-
iers is not made.  The kinds of external and static identifiers which  will  not
be  initialized  properly  are  those  that  are  explicitly-initialized with an
expression which contains:

     a)  A string.  (Except in the case where it is used to initialize an  array
     identifier.)  An example of this is:

                     char *string = "string";
                     main() {printf("string equals %s\n",string);}


     b)  A previously-declared identifier. An example of this is:

                     int a[1] = {12};
                     static int *b = a;
                     main() {printf("pointer to b equals %d\n",*b);}

WORKAROUND: Make the first statement in your 'main' function the following:

                     asm("cxp %initfun");

Please note that the above need only be done once per program.


_C_S_H


For an account running the maximum number of processes allowable,  CSH  will
not  issue  a  "No more processes" error message when another user logs into
this account.  Instead, the new user's process will hang up as  soon  as  he
enters  his  first command.  WORKAROUND: Do not login to an account which is
running 15 processes.



_D_D_T


Floating-point registers used as operands in floating-point instructions are
disassembled as general purpose registers.  WORKAROUND: None.




The dummy operands of the 'LFSR' and 'SFSR' instructions are mistakenly  in-
cluded  in  the  display, as 'R0', when DDT disassembles either of these two
instructions.  WORKAROUND: None, except ignoring the erroneous part  of  the
output.




                                 433010137-001                     Page 11 of 93


                                  GENIX SOURCE


The value of the 'CMPM' instruction's 'length' operand is always disassembled as
'0'.
WORKAROUND: None, except decoding the operand yourself.


Use of the '$P' Remote Debugging Command does not always result in  the  program
stopping after one nonsequential step has been performed.  WORKAROUND: None.


The command string associated with an MMU breakpoint is not  executed  when  the
breakpoint is encountered.  WORKAROUND: None.


DDT cannot consistently display the correct value  of  a  floating-point  number
when in the '$mf' output format mode.  WORKAROUND: None.


When 'addr\' is specified, DDT displays the value of the current memory location
rather  than the memory location of 'addr'.  WORKAROUND: Use 'addr/'.  This will
display the value you want, but the current memory location will change to  that
of 'addr'.


The use of 'addr!value' does result in 'value' being assigned to 'addr', but the
current memory location is not changed to 'addr'.  WORKAROUND: Use 'addr/value'.
The current value will be displayed before it is changed.

_D_U_M_P


The file system that is dumped by default upon invocation of the DUMP command is
incorrect.   WORKAROUND:  Explicitly  specify  the  file  system to be dumped as
'/dev/rdc1a'.


_F_L_O_A_T_I_N_G-_P_O_I_N_T _S_U_P_P_O_R_T


Because of NS16032 scaled-index-addressing issues (which are  documented  in
the  "USER  INFORMATION"  sheet provided with the component), the user could
encounter erroneous results when he attempts to access elements in arrays of
type 'float' or 'double'.
WORKAROUND: For example, instead of doing:

                main ()
                {
                         float s[10];
                         register int index;

                         for(index = 0;index < 10; index++)
                             {
                               s[index] = 3;
                               printf("Value     of     element     %d     =
                      %g\n",index,s[index]);
                             }
                }




                                 433010137-001                     Page 12 of 93


                                  GENIX SOURCE


do the following:


                main ()
                {
                         float s[10];
                         register float *sptr,*eptr;
                         register int index;

                         eptr = s+10;
                         for(index = 0,sptr = s;sptr < eptr; index++,sptr++)
                             {
                               *sptr = 3;
                               printf("Value of element %d = %g\n",index,*sptr);
                             }
                }



Inaccurate results may be obtained when the user runs a  program  that  performs
floating-point  operations  that  will be executed by a revision 'C' or lower of
the NS16081 chip.
WORKAROUND: Obtain a 'C1' or higher revision of the chip.

_L_D


The link editor aborts when a superfluous library is specified before  a  manda-
tory library.
WORKAROUND: Do not use unnecessary libraries.


_M_A_T_H _L_I_B_R_A_R_Y


Executing a program utilizing the 'FABS' function results in a "Floating ex-
ception" error message.  WORKAROUND: Create a '*.s' file containing the fol-
lowing NS16000 instructions:

                _fabs::
                         absl 8(sp),f0
                         rxp 0

After the above file has been created, then use 'AS' to generate  an  object
file  from  it.  This object file should then be linked to any program which
makes use of the 'FABS' function.  An example of this last point is:

                cc usesfabs.c fabs.o

Where, 'usesfabs.c' is a C program that makes use of the 'FABS' function and
sembler program.


_N_R_O_F_F


NROFF may insert a few garbage bytes into the output when it processes  a  list.
This  is  most  likely  to  occur  when the terminal type specified using the -T


                                 433010137-001                     Page 13 of 93


                                  GENIX SOURCE


option is one of the following: dm, h2, lpr, lpr2 or tn300.  WORKAROUND:  Use  a
terminal  type other than those just specified or re-direct the output to a file
and edit the garbage out from it before printing it.


_S_T_A_N_D-_A_L_O_N_E _I/_O _L_I_B_R_A_R_Y


In a DB16000 (or equivalent) environment, an integer value cannot be printed
out  using  a  minimum-field-width  digit string in conjunction with the 'd'
conversion character.
WORKAROUND: Do not specify a minimum-field-width digit string  for  the  'd'
conversion character.


_T_A_R


The -r and -u options do not work.  WORKAROUND: Your only alternative is to  use
'DUMP' more often.

_G_E_N_I_X _P_R_O_G_R_A_M_M_E_R'_S _M_A_N_U_A_L, _V_O_L_U_M_E _1


The manual page for 'CC(1)' says that the -g option can also be  passed  to  the
assembler  to  tell  it  to  generate additional symbol table data.  This is not
true.  This option only causes  line  numbers  to  be  placed  in  the  assembly
language source file of a compiled C program when the -S option is also used.


The manual page for 'CU16(1)' says that the '-b' option can be used upon invoca-
tion  to  tell CU16 to change nulls into breaks.  This is not true.  This option
is not supported by CU16  and  any  documentation  to  the  contrary  should  be
ignored.


The manual page for 'DDT(1)' needs the following revisions, warnings  and  addi-
tions:


     a)  The 'Symbols' Section

        REVISION: This entire section should be replaced by the following text:

              Symbols are composed of  alphanumerics,  underscores  (_),  dollar
              signs ($), and periods (.).

              There are two cases where the '.' character is used  to  represent
              something other than a period:

                    1)  When '.' appears by itself as an argument in  a  command
                    line, it represents the address last specified in a previous
                    command.

                    2)  When '.' is preceded by a string composed of one or more
                    digits,  it  represents  an operator that tells DDT that the
                    digit string should be interpreted as a decimal value.



                                 433010137-001                     Page 14 of 93


                                  GENIX SOURCE


              The '$' character is an anomaly of sorts.  When it  appears  as  a
              character  of  a  symbol  composed  of  two or more characters, it
              represents the '.' character, i.e., '$' is an alias  for  '.'.   A
              symbol  composed  of  only the '$' character is not possible since
              the assembler does not accept such a symbol as legal.

              Predefined symbols available to the user independent of  the  pro-
              gram being debugged represent the registers of the NS16000 Family:


              r0, r1, r2, r3, r4, r5, r6 ,r7   General Purpose Registers 0-7

              f0, f1, f2, f3, f4, f5, f6 ,f7   Floating-Point Registers 0-7 (local)

              psr                              Processor Status Register

              mod                              Module Table Register

              sb                               Static Base Register

              pc                               Program Counter Register

              sp0                              Interrupt Stack Pointer Register

              sp1                              User Stack Pointer Register

              fp                               Frame Pointer Register

              intbase                          Interrupt Base Register


              The above names  are  consistent  with  those  documented  in  the
              NS16032  data  sheet.   Also,  please note that the Floating-Point
              Registers cannot be accessed during a remote debugging session.

              Other symbols available to the user are dependent upon the program
              being debugged.

              When using a symbol in a command line, one need not specify it  in
              its  entirety.  The symbol can be abbreviated in two basic ways or
              it can be  specified completely.  The ways in which a  symbol  can
              be abbreviated are:

                    1)  The first part of the symbol can be  specified  and  the
                    rest  of  it  can  be  left  undefined,  i.e., type in 'SYM'
                    instead of 'SYMBOL'.

                    2)  The first character of the symbol can be  left  unspeci-
                    fied  if  it is the underscore (_) character and the rest of
                    the symbol can either be  completely  or  partly  specified,
                    i.e., type in 'SYM' or 'SYMBOL' instead of '_SYMBOL'.

              One thing you should keep in mind when abbreviating  a  symbol  is
              the  symbol search algorithm DDT uses so that the symbol DDT finds
              matches the symbol you intended by your  abbreviated  form.   When
              searching for a symbol, DDT uses the following rules:

                    1)  If the symbol does not begin with an underscore and  the


                                 433010137-001                     Page 15 of 93


                                  GENIX SOURCE


                    symbol table symbol found matches it exactly, then terminate
                    the search, e.g., 'SYM' is specified and 'SYM' is  a  symbol
                    in the symbol table.  Otherwise, see if rule #2 is true.

                    2)  If the symbol does begin with an underscore and the sym-
                    bol  table  symbol found matches it exactly or if the symbol
                    does not begin with an underscore but a symbol table  symbol
                    beginning  with  an  underscore  matches  it  exactly if the
                    underscore is not taken into consideration,  then  terminate
                    the search, e.g., '_SYM' or 'SYM' is specified and '_SYM' is
                    a symbol in the symbol table.  Otherwise, see if rule #3  is
                    true.

                    3)  If the symbol does not begin with an underscore and  the
                    beginning  part  of the symbol table symbol found matches it
                    exactly, then terminate the search, e.g., 'SYM' is specified
                    and  'SYMBOL'  is  a symbol in the symbol table.  Otherwise,
                    see if rule #4 is true.

                    4)  If the symbol does begin  with  an  underscore  and  the
                    beginning  part  of the symbol table symbol found matches it
                    exactly or if the symbol does not begin with  an  underscore
                    but  a  symbol  table  symbol  beginning  with an underscore
                    matches it exactly if its underscore is not taken into  con-
                    sideration,  then  terminate the search, e.g., symbol table.
                    Otherwise, print out the following message:

                            symbol <SYMBOL-SPECIFIED> not found

              Please keep in mind that if DDT returns '_SYMBOL'  when  'SYM'  is
              specified that this does not necessarily mean that 'SYM' or '_SYM'
              are not present in the symbol table.  What it may mean is that DDT
              encountered  '_SYMBOL'  before  'SYM'  or  '_SYM'  when  doing its
              search.


     b)  The '$mi' Mode Selection Command

        1.  WARNING: When disassembled, the value displayed for the 'length'
        operand  of  the 'MOVM' instruction will not correspond to the value
        specified in the assembler source file.  Instead, DDT  displays  the
        value  of  the 'length' operand as it was encoded in the machine in-
        struction.

        2.  WARNING: Floating-point constants cannot be  disassembled.   En-
        countering  one  such constant during disassembly causes DDT to mis-
        translate the rest of the current  instruction  and  subsequent  in-
        structions.












                                 433010137-001                     Page 16 of 93


                                  GENIX SOURCE



     c)  The "Program Control Commands"

        1.  'addr$b' --- ADDITION: The total number of breakpoints which can
        be set at any one time is 15.

        2.  'number$ecommand-string'  ---  ADDITION:  The  total  number  of
        breakpoints which can be assigned a command string is 10.


     d)  The "Remote Debugging Commands"

        1.  WARNING: When using the DB1600 during a remote debugging session, be
        sure not to execute a program which outputs any of the following charac-
        ters: 'E', '=', '?' or '*'.  Otherwise, your program will not execute to
        completion.

        2.  WARNING: DDT does not have a command for changing  the  contents  of
        the NS16032's configuration register during a remote debugging session.

        3.  'addr$x' ---

           a)  ADDITION: The total number of MMU breakpoints which can be set at
           any one time is 2.

           b)  WARNING: The message DDT displays upon encountering an MMU break-
           point  is  very  poor.   The  message DDT displays for other types of
           breakpoints is much better.


The manual page for 'DUMP(8)' needs to be revised so that the first line of  the
'FILES'   section   states  that  the  "default  filesystem  to  dump  from"  is
'/dev/rdc1a' instead of '/dev/rdc1g'.



























                                 433010137-001                     Page 17 of 93


                                  GENIX SOURCE



The manual page for 'FILSYS(5)' has the wrong 'include' files  in  it.   The
ones  in  the text are the 4.1bsd versions.  The GENIX versions of these in-
clude files are as follows:

                               <_s_y_s/_f_i_l_s_y_s._h>

          /*
           * @(#)filsys.h 3.3               7/14/83
           * @(#)Copyright (C) 1983 by National Semiconductor Corp.
           */

          /*
           * Structure of the super-block
           */
          struct filsys
          {
                 unsigned short s_isize;    /* size in blocks of i-list */
                 daddr_t  s_fsize;          /* size in blocks of entire volume */
                 short    s_nfree;          /* number of addresses in s_free */
                 daddr_t  s_free[NICFREE];  /* free block list */
                 short    s_ninode;         /* number of i-nodes in s_inode */
                 ino_t    s_inode[NICINOD]; /* free i-node list */
                 char     s_flock;          /* lock during free list manipulation */
                 char     s_ilock;          /* lock during i-list manipulation */
                 char     s_fmod;           /* super block modified flag */
                 char     s_ronly;          /* mounted read-only flag */
                 time_t   s_time;           /* last super block update */
                 daddr_t  s_tfree;          /* total free blocks*/
                 ino_t    s_tinode;         /* total free inodes */
                 /* begin not maintained by this version of the system */
                 short    s_dinfo[2];       /* interleave stuff */
          #define         s_m               s_dinfo[0]
          #define         s_n               s_dinfo[1]
                 /* end not maintained */
                 char     s_fname[12];      /* file system name */
                 ino_t    s_lasti;          /* start place for circular search */
                 ino_t    s_nbehind;        /* est # free inodes before s_lasti */
          };

          #ifdef KERNEL
          struct filsys *getfs();
          #endif

















                                 433010137-001                     Page 18 of 93


                                  GENIX SOURCE


                                  <_s_y_s/_f_b_l_k._h>

              /*
               * @(#)fblk.h   3.2               7/14/83
               * @(#)Copyright (C) 1983 by National Semiconductor Corp.
               */

              struct fblk
              {
                     int      df_nfree;
                     daddr_t  df_free[NICFREE];
              };



                                  <_s_y_s/_i_n_o._h>


              /*
               * @(#)ino.h    3.2          7/14/83
               * @(#)Copyright (C) 1983 by National Semiconductor Corp.
               */

              /*
               * Inode structure as it appears on
               * a disk block.
               */
              struct dinode
              {
                      unsigned short       di_mode;     /* mode and type of file */

                      short   di_nlink;    /* number of links to file */
                      short   di_uid;      /* owner's user id */
                      short   di_gid;      /* owner's group id */
                      off_t   di_size;     /* number of bytes in file */
                      char    di_addr[40]; /* disk block addresses */
                      time_t  di_atime;    /* time last accessed */
                      time_t  di_mtime;    /* time last modified */
                      time_t  di_ctime;    /* time created */
              };
              #define INOPB   16           /* 16 inodes per BSIZE block */
              /*
               * the 40 address bytes:
               *      39 used; 13 addresses
               *      of 3 bytes each.
               */



The manual page for 'LEX(1)' has an error in third line of the sample program in
the  'DESCRIPTION' section. The line should be terminated by a semi-colon, e.g.,
'[ ]+$;'.


The manual page for 'LD(1)' requires the following supplements:

     a)  The specification of options upon invocation --- To avoid any  unneces-
     sary  headaches,  all options, barring '-l', should be specified before any


                                 433010137-001                     Page 19 of 93


                                  GENIX SOURCE


     'module'.  Doing so will insure the data integrity of  your  'a.out'  file.
     Not  doing  so will result in unnecessary errors being encountered when the
     executable file is run.

     b)  The '-R' option --- First, this option is used to exclude  the  program
     segment  of  the  text  segment  of the 'module' from the image while still
     retaining its link table. Second, the value of 'address' also needs  defin-
     ing: 'address' should be the address of the 'module's text segment.

     c)  The '-r' option --- When this option is specified, the user should  not
     also specify the '-T' option since the two options are mutually exclusive.


The manual page for 'NBURN(1)' says that "Nburn prints the  test  size  and  the
data  size,  for  the  whole  file."   This is not true.  NBURN prints the "text
size", not the "test size," of the file.



The manual page for 'PS(1)' needs to be revised so that its first  paragraph
is as follows:

     Ps prints information about processes.  Normally, only your  processes,
     except  ps  itself,  are candidates to be printed by ps.  By specifying
     'a', other user's processes become candidates to be printed in addition
     to  yours.   Note that 'a' also causes ps itself to be printed since in
     GENIX ps's effective  id  is  'root'.   By  specifying  'x',  processes
     without control terminals are also placed in the candidate pool.




The manual page for 'REBOOT(2V)' needs to be revised in order to correct the
mistakes  made  in  the  first paragraph of its 'DESCRIPTION' section.  This
paragraph should be as follows:

     Reboot is used to cause a system reboot and, in the event of  an  unre-
     coverable system failure, it is invoked automatically.  Howto is a mask
     of options passed to the bootstrap program.  The system call  interface
     permits only the RB_HALT or RB_AUTOBOOT options to be passed to the re-
     boot program.  When neither of these options are passed, the system  is
     automatically  rebooted from the file 'vmunix'.  'vmunix' is located in
     the root file system on disk unit 1, i.e., /dev/dc1a.




The manual page for 'SYSCALL(2)' needs to be revised in order to correct the
mistake  made  in  the  second paragraph of its 'DESCRIPTION' section.  This
paragraph should be as follows: "The value generated by the system  call  is
returned in the register R0."



The manual page for 'TAR(1)' needs the following revisions:

     a)  The paragraph for '0,...,7' should state,  "This  modifier  selects  an
     alternative drive on which the tape is mounted.  (The default is drive 0 at


                                 433010137-001                     Page 20 of 93


                                  GENIX SOURCE


     1600 bpi, which is normally /dev/nrtc.)"

     b)  The data listed in the 'FILES' section should be revised so that it  is
     as follows:

           /dev/rtc*
           /dev/nrtc*
           /tmp/tar*


_G_E_N_I_X _P_R_O_G_R_A_M_M_E_R'_S _M_A_N_U_A_L, _V_O_L_U_M_E _2


_G_E_N_I_X _A_s_s_e_m_b_l_e_r _R_e_f_e_r_e_n_c_e _M_a_n_u_a_l --- _S_e_p_t_e_m_b_e_r, _1_9_8_3


Footnote number 2 on page 1 erroneously states that "VAX is a trademark of Digi-
tal Reasearch Corporation."  The statement should instead state "VAX is a trade-
mark of Digital Equipment Corporation."


Section 9, "Assembler Directives", does not document the '.EQU' directive.  This
directive belongs to a new directive class called "Symbol-Creation."  The syntax
of the directive is:

     <symbol> : [ : ] .EQU <expression>

     where:
           <symbol>  is a name of a symbol created in conformance with the  con-
           ventions documented in section 3, "Symbol Construction".

           '.EQU' is the name of the directive.

           <expression>  is an expression as  defined  in  section  6,  'Expres-
           sions'.  Please note that character strings are limited to contain at
           most 4 members. A symbol thus specified will be treated as an integer
           value  where  the right-most character's ASCII value becomes the most
           significant part of the integer value and so  on  for  any  remaining
           characters.    For  example,  the  character  string  'AB'  would  be
           equivalent to an integer constant having a value of h'4241.

     The '.EQU' directive defines a symbol whose  value  is  determined  by  the
     value of the '<expression>'.  The scope of a symbol may be:

           Local --- The symbol may be used anywhere within  the  module  it  is
           defined in.  Examples of these local symbols are:

                 IS$RRELSB:    .EQU 5(SB)
                 IS$R1:           .EQU R1
                 IS$MRELFP:   .EQU 1(2(FP))

           Global --- The symbol may be used anywhere within the  module  it  is
           defined  in  and  the symbol will be entered in the executable file's
           symbol table for debug purposes.  (Please note  that  this  does  not
           mean that such a symbol can be '.IMPORT'ed by another module.)  Exam-
           ples of these global symbols are:

                 IS$EXIMM::    .EQU 32768


                                 433010137-001                     Page 21 of 93


                                  GENIX SOURCE


                 IS$EXABS::    .EQU @h'8000
                 IS$EXRREL::  .EQU 17(R0)


Section 9.3.2, ".BLKB, .BLKW,  .BLKD,  .BLKF,  .BLKL;"  does  not  mention  that
'count'  may  be a character string.  This string is treated by the Assembler as
an integer value and because of this it may not contain  more  than  4  members.
The  integer  value  of a string is computed by using the right-most character's
ASCII value as the most significant part of the integer value and so on for  any
remaining   characters.   For  example,  the  character  string  'AB'  would  be
equivalent to an integer constant having a value of h'4241.


Section 9.4.5, '.WIDTH,' does not mention that a default value for in the  range
80 to 132, inclusive.  This default value is 132.

_D_b_m_o_n _R_e_f_e_r_e_n_c_e _M_a_n_u_a_l --- _S_e_p_t_e_m_b_e_r, _1_9_8_3


Section 1., 'Introduction,' has an erroneous footnote reference  for  'UNIX'  in
it.   The reference is made to footnote number '1,' whereas the correct footnote
number is '2.'






































                                 433010137-001                     Page 22 of 93


                                  GENIX SOURCE
















                      _N_A_T_I_O_N_A_L _S_E_M_I_C_O_N_D_U_C_T_O_R _U_S_E_R-_S_U_P_P_O_R_T



                          _C_a_l_i_f_o_r_n_i_a:  (800) 672-1811

                       _C_o_n_t_i_n_e_n_t_a_l _U._S.:  (800) 538-1866

                 _O_t_h_e_r:  _C_o_n_t_a_c_t _Y_o_u_r _L_o_c_a_l _N_S_C _R_e_p_r_e_s_e_n_t_a_t_i_v_e





































                                 433010137-001                     Page 23 of 93


                                  GENIX SOURCE


S_o_f_t_w_a_r_e__P_r_o_d_u_c_t__D_o_c_u_m_e_n_t_a_t_i_o_n__S_u_p_p_l_e_m_e_n_t_



 _D_o_c_u_m_e_n_t:  SYS16 Systems Manual

 _R_e_v_i_s_i_o_n:  DRAFT


A hard-copy of the above manual has been included in this GENIX  source  release
package.   Also,  a  copy of chapter 5 of this manual, with embedded -me macros,
has been included as a file on the GENIX package's third magnetic tape.
















































                                 433010137-001                     Page 24 of 93


                                  GENIX SOURCE


_F_i_l_e _L_i_s_t_i_n_g_s


_T_a_p_e _1:  _G_E_N_I_X_1._0_A_I

 ./dev:
 total 6
 drwxrwxrwx 2 1056       1616 Sep 27 15:44 ./
 drwxrwxr-x 3 1056        928 Dec 15 16:39 ../
 crw-rw-rw- 1 root      0,  0 May 22  1982 console
 brw------- 1 root      0,  0 Jun 20 17:31 dc0a
 brw------- 1 root      0,  1 Jun 20 17:32 dc0b
 brw------- 1 root      0,  2 Jun 20 17:32 dc0c
 brw------- 1 root      0,  3 Sep 15 15:26 dc0d
 brw------- 1 root      0,  4 Sep 15 15:27 dc0e
 brw------- 1 root      0,  5 Sep 15 15:28 dc0f
 brw------- 1 root      0,  6 Jun 20 17:32 dc0g
 brw------- 1 root      0,  7 Sep 15 15:28 dc0h
 brw------- 1 root      0,  8 Jun  9  1982 dc1a
 brw------- 2 root      0,  9 Jun  9  1982 dc1b
 brw------- 1 root      0, 10 Sep 25  1982 dc1c
 brw------- 1 root      0, 11 Sep 15 15:29 dc1d
 brw------- 1 root      0, 12 Sep 15 15:29 dc1e
 brw------- 1 root      0, 13 Sep 15 15:30 dc1f
 brw------- 1 root      0, 14 Jun  7  1982 dc1g
 brw------- 1 root      0, 15 Sep 15 15:30 dc1h
 brw------- 1 root      0, 16 Sep 15 15:30 dc2a
 brw------- 1 root      0, 17 Sep 15 15:31 dc2b
 brw------- 1 root      0, 18 Sep 15 15:31 dc2c
 brw------- 1 root      0, 19 Sep 15 15:31 dc2d
 brw------- 1 root      0, 20 Sep 15 15:31 dc2e
 brw------- 1 root      0, 21 Sep 15 15:31 dc2f
 brw------- 1 root      0, 22 Sep 15 15:32 dc2g
 brw------- 1 root      0, 23 Sep 15 15:32 dc2h
 brw------- 1 root      0, 24 Sep 15 15:32 dc3a
 brw------- 1 root      0, 25 Sep 15 15:32 dc3b
 brw------- 1 root      0, 26 Sep 15 15:32 dc3c
 brw------- 1 root      0, 27 Sep 15 15:32 dc3d
 brw------- 1 root      0, 28 Sep 15 15:33 dc3e
 brw------- 1 root      0, 29 Sep 15 15:33 dc3f
 brw------- 1 root      0, 30 Sep 15 15:33 dc3g
 brw------- 1 root      0, 31 Sep 15 15:33 dc3h
 brw------- 2 root      0,  9 Jun  9  1982 drum
 crw-r--r-- 1 root      1,  1 Jun 12  1982 kmem
 crw-rw-rw- 1 root      9,  2 Sep 27 15:44 lp
 crw-r--r-- 1 root      1,  0 Jun 12  1982 mem
 crw-rw-rw- 2 root      6,  4 Nov  9  1982 nrtc0
 brw-rw-rw- 2 root      1,  4 Nov  9  1982 ntc0
 crw-rw-rw- 1 root      1,  2 Jun 12  1982 null
 crw-rw-rw- 1 root      5,  0 Nov  3  1982 pty0
 crw-rw-rw- 1 root      5,  1 Nov  5  1982 pty1
 crw-rw-rw- 1 root      5,  2 Nov  5  1982 pty2
 crw-rw-rw- 1 root      5,  3 Nov  5  1982 pty3
 crw-rw-rw- 1 31        5,  4 Nov  5  1982 pty4
 crw-rw-rw- 1 root      5,  5 Nov  5  1982 pty5
 crw------- 1 root      2,  0 Jun 20 17:32 rdc0a
 crw------- 1 root      2,  1 Jun 20 17:32 rdc0b
 crw------- 1 root      2,  2 Jun 20 17:33 rdc0c


                                 433010137-001                     Page 25 of 93


                                  GENIX SOURCE


 crw------- 1 root      2,  3 Sep 15 15:38 rdc0d
 crw------- 1 root      2,  4 Sep 15 15:38 rdc0e
 crw------- 1 root      2,  5 Sep 15 15:39 rdc0f
 crw------- 1 root      2,  6 Jun 20 17:33 rdc0g
 crw------- 1 root      2,  7 Sep 15 15:39 rdc0h
 crw------- 1 root      2,  8 Aug 21  1982 rdc1a
 crw------- 1 root      2,  9 Aug 21  1982 rdc1b
 crw------- 1 root      2, 10 Jan 20  1983 rdc1c
 crw------- 1 root      2, 11 Sep 15 15:39 rdc1d
 crw------- 1 root      2, 12 Sep 15 15:39 rdc1e
 crw------- 1 root      2, 13 Sep 15 15:40 rdc1f
 crw------- 1 root      2, 14 Aug 21  1982 rdc1g
 crw------- 1 root      2, 15 Sep 15 15:40 rdc1h
 crw------- 1 root      2, 16 Sep 15 15:40 rdc2a
 crw------- 1 root      2, 17 Sep 15 15:41 rdc2b
 crw------- 1 root      2, 18 Sep 15 15:41 rdc2c
 crw------- 1 root      2, 19 Sep 15 15:41 rdc2d
 crw------- 1 root      2, 20 Sep 15 15:41 rdc2e
 crw------- 1 root      2, 21 Sep 15 15:41 rdc2f
 crw------- 1 root      2, 22 Sep 15 15:42 rdc2g
 crw------- 1 root      2, 23 Sep 15 15:42 rdc2h
 crw------- 1 root      2, 24 Sep 15 15:42 rdc3a
 crw------- 1 root      2, 25 Sep 15 15:42 rdc3b
 crw------- 1 root      2, 26 Sep 15 15:43 rdc3c
 crw------- 1 root      2, 27 Sep 15 15:43 rdc3d
 crw------- 1 root      2, 28 Sep 15 15:43 rdc3e
 crw------- 1 root      2, 29 Sep 15 15:43 rdc3f
 crw------- 1 root      2, 30 Sep 15 15:43 rdc3g
 crw------- 1 root      2, 31 Sep 15 15:43 rdc3h
 crw-rw-rw- 1 root      6,  0 Nov  9  1982 rtc0
 crw-rw-rw- 2 root      6,  4 Nov  9  1982 rtc4
 brw-rw-rw- 1 root      1,  0 Nov  9  1982 tc0
 brw-rw-rw- 2 root      1,  4 Nov  9  1982 tc4
 crw-rw-rw- 1 root      3,  1 Nov 21 16:20 tty
 crw-rw-rw- 1 root      8,  0 Dec 27  1982 tty00
 crw-rw-rw- 1 root      8,  1 Dec 27  1982 tty01
 crw-rw-rw- 1 root      8,  2 Dec 27  1982 tty02
 crw-rw-rw- 1 root      8,  3 Dec 27  1982 tty03
 crw-rw-rw- 1 root      8,  4 Dec 27  1982 tty04
 crw-rw-rw- 1 root      8,  5 Dec 27  1982 tty05
 crw-rw-rw- 1 root      8,  6 Dec 27  1982 tty06
 crw-rw-rw- 1 root      8,  7 Dec 27  1982 tty07
 crw-rw-rw- 1 root      7,  0 Dec 27  1982 tty08
 crw-rw-rw- 1 root      4,  0 Nov  3  1982 ttyp0
 crw-rw-rw- 1 root      4,  1 Nov  5  1982 ttyp1
 crw-rw-rw- 1 root      4,  2 Nov  5  1982 ttyp2
 crw-rw-rw- 1 root      4,  3 Nov  5  1982 ttyp3
 crw-rw-rw- 1 root      4,  4 Nov  5  1982 ttyp4
 crw-rw-rw- 1 root      4,  5 Nov  5  1982 ttyp5

 rwxr-xr-x1204/70      0 Dec 15 17:03 1983 src/
 rwxr-xr-x1204/70      0 Dec 15 16:47 1983 src/sys/
 rwxr-xr-x1204/70      0 Dec 15 16:46 1983 src/sys/stand/
 rw-r--r--1204/70   4684 Oct 10 16:38 1983 src/sys/stand/makefile
 rw-r--r--1204/70   2800 Dec  9 18:55 1983 src/sys/stand/mkstand
 rw-r--r--1204/70    955 Oct 14 14:23 1983 src/sys/stand/conf.c
 rw-r--r--1204/70    678 Oct 14 14:24 1983 src/sys/stand/ls.c
 rw-r--r--1204/70   4003 Oct 14 14:23 1983 src/sys/stand/dc.c


                                 433010137-001                     Page 26 of 93


                                  GENIX SOURCE


 rw-r--r--1204/70  11534 Oct 18 16:40 1983 src/sys/stand/dcusaio.c
 rw-r--r--1204/70   8732 Oct 14 14:23 1983 src/sys/stand/tcusaio.c
 rw-r--r--1204/70   4802 Oct 14 14:23 1983 src/sys/stand/dcusize.c
 rw-r--r--1204/70    180 Oct 14 14:23 1983 src/sys/stand/fake.c
 rw-r--r--1204/70    499 Oct 14 14:23 1983 src/sys/stand/gets.c
 rw-r--r--1204/70   1666 Oct 14 14:23 1983 src/sys/stand/ie.c
 rw-r--r--1204/70  10388 Oct 14 14:23 1983 src/sys/stand/ieeio.c
 rw-r--r--1204/70   1200 Oct 14 14:23 1983 src/sys/stand/prf.c
 rw-r--r--1204/70   4090 Oct 14 14:23 1983 src/sys/stand/rs232.c
 rw-r--r--1204/70   8774 Oct 14 14:23 1983 src/sys/stand/sys.c
 rw-r--r--1204/70   4067 Oct 14 14:23 1983 src/sys/stand/tc.c
 rw-r--r--1204/70   2487 Aug 30 16:58 1983 src/sys/stand/diskcopy.c
 rw-r--r--1204/70   2812 Oct 14 14:23 1983 src/sys/stand/boot.c
 rw-r--r--1204/70    421 Oct 14 14:23 1983 src/sys/stand/cat.c
 rw-r--r--1204/70   3243 Oct 14 14:23 1983 src/sys/stand/copy.c
 rw-r--r--1204/70   3196 Oct 14 14:24 1983 src/sys/stand/mkpt.c
 rw-r--r--1204/70  39609 Oct 18 16:55 1983 src/sys/stand/dcutest.c
 rw-r--r--1204/70    426 Oct 14 14:24 1983 src/sys/stand/atol.c
 rw-r--r--1204/70    430 Oct 14 14:24 1983 src/sys/stand/l3tol.c
 rw-r--r--1204/70    420 Oct 14 14:24 1983 src/sys/stand/ltol3.c
 rw-r--r--1204/70   1596 Oct 14 14:24 1983 src/sys/stand/qsort.c
 rw-r--r--1204/70    350 Oct 14 14:24 1983 src/sys/stand/strcpy.c
 rw-r--r--1204/70   1009 Oct 14 14:24 1983 src/sys/stand/setjmp.s
 rw-r--r--1204/70   9646 Oct 14 14:24 1983 src/sys/stand/icheck.c
 rw-r--r--1204/70  10700 Oct 14 14:24 1983 src/sys/stand/mkfs.c
 rw-r--r--1204/70  24420 Oct 14 14:24 1983 src/sys/stand/restor.c
 rw-r--r--1204/70   5544 Oct 14 14:25 1983 src/sys/stand/bootsrt0.s
 rw-r--r--1204/70   5683 Oct 14 14:25 1983 src/sys/stand/ebootsrt0.s
 rw-r--r--1204/70    604 Oct 14 14:25 1983 src/sys/stand/iomove.s
 rw-r--r--1204/70    220 Oct 14 14:25 1983 src/sys/stand/rtt.s
 rw-r--r--1204/70    607 Oct 14 14:25 1983 src/sys/stand/ss.s
 rw-r--r--1204/70   8442 Sep 19 15:33 1983 src/sys/stand/tags
 rw-r--r--1204/70    685 Aug 19 09:39 1983 src/sys/stand/README
 rw-r--r--1204/70   1740 Aug 19 15:39 1983 src/sys/stand/dcutest.help
 rwxr-xr-x1204/70      0 Dec 15 16:46 1983 src/sys/dev/
 rw-r--r--1204/70   2178 Aug 18 21:53 1983 src/sys/dev/makefile
 rw-r--r--1204/70  13051 Sep  6 16:13 1983 src/sys/dev/bio.c
 rw-r--r--1204/70   4557 Aug 18 21:53 1983 src/sys/dev/bm.c
 rw-r--r--1204/70   3873 Nov  9 17:03 1983 src/sys/dev/conf.c
 rw-r--r--1204/70  25657 Nov  8 23:01 1983 src/sys/dev/dcu.c
 rw-r--r--1204/70   2849 Aug 18 21:54 1983 src/sys/dev/dsort.c
 rw-r--r--1204/70   8219 Aug 18 21:54 1983 src/sys/dev/lp.c
 rw-r--r--1204/70   3264 Oct 10 13:34 1983 src/sys/dev/mem.c
 rw-r--r--1204/70  10155 Aug 31 00:20 1983 src/sys/dev/tty.c
 rw-r--r--1204/70   8723 Aug 22 17:54 1983 src/sys/dev/rs.c
 rw-r--r--1204/70  25300 Oct  3 11:05 1983 src/sys/dev/sio.c
 rw-r--r--1204/70  24333 Oct 13 16:55 1983 src/sys/dev/tcu.c
 rw-r--r--1204/70  37468 Aug 18 21:54 1983 src/sys/dev/termem.c
 rw-r--r--1204/70  18094 Aug 31 00:17 1983 src/sys/dev/ttynew.c
 rw-r--r--1204/70   6114 Aug 27 14:03 1983 src/sys/dev/pty.c
 rw-r--r--1204/70  11229 Oct 19 18:40 1983 src/sys/dev/flavors.c
 rw-r--r--1204/70  21172 Aug 18 21:54 1983 src/sys/dev/blt.s
 rw-r--r--1204/70  15344 Aug 14 18:21 1983 src/sys/dev/Typerite_10.s
 rwxr-xr-x1204/70      0 Dec 15 16:46 1983 src/sys/sys/
 rw-r--r--1204/70   3675 Aug 22 13:38 1983 src/sys/sys/makefile
 rw-r--r--1204/70   2100 Aug 18 21:51 1983 src/sys/sys/acct.c
 rw-r--r--1204/70   9976 Aug 18 21:51 1983 src/sys/sys/alloc.c
 rw-r--r--1204/70  10784 Aug 21 10:32 1983 src/sys/sys/clock.c


                                 433010137-001                     Page 27 of 93


                                  GENIX SOURCE


 rw-r--r--1204/70   4989 Aug 18 21:51 1983 src/sys/sys/fio.c
 rw-r--r--1204/70   2622 Aug 22 17:53 1983 src/sys/sys/genassym.c
 rw-r--r--1204/70   8570 Aug 18 21:52 1983 src/sys/sys/iget.c
 rw-r--r--1204/70   1708 Oct 18 16:04 1983 src/sys/sys/ioctl.c
 rw-r--r--1204/70  10219 Sep 11 15:31 1983 src/sys/sys/machdep.c
 rw-r--r--1204/70   7717 Nov 13 11:56 1983 src/sys/sys/slp.c
 rw-r--r--1204/70   4461 Oct  3 11:00 1983 src/sys/sys/nami.c
 rw-r--r--1204/70   4004 Nov  9 17:20 1983 src/sys/sys/param.c
 rw-r--r--1204/70    842 Aug 18 21:52 1983 src/sys/sys/partab.c
 rw-r--r--1204/70   3732 Aug 18 21:52 1983 src/sys/sys/pipe.c
 rw-r--r--1204/70   8522 Sep 28 14:27 1983 src/sys/sys/prf.c
 rw-r--r--1204/70   6187 Aug 18 21:52 1983 src/sys/sys/prim.c
 rw-r--r--1204/70   4344 Aug 18 21:52 1983 src/sys/sys/rdwri.c
 rw-r--r--1204/70   5318 Nov  9 12:17 1983 src/sys/sys/main.c
 rw-r--r--1204/70  15302 Aug 29 14:41 1983 src/sys/sys/sig.c
 rw-r--r--1204/70   7514 Oct  3 12:10 1983 src/sys/sys/trap.c
 rw-r--r--1204/70  15152 Nov 13 11:56 1983 src/sys/sys/sys1.c
 rw-r--r--1204/70   2377 Nov 13 12:00 1983 src/sys/sys/stash.c
 rw-r--r--1204/70   4338 Aug 18 21:52 1983 src/sys/sys/subr.c
 rw-r--r--1204/70  18827 May  5 12:24 1983 src/sys/sys/tags
 rw-r--r--1204/70   1797 Aug 18 21:52 1983 src/sys/sys/sufu.c
 rw-r--r--1204/70   4881 Aug 30 23:19 1983 src/sys/sys/swtch.c
 rw-r--r--1204/70   1016 Aug 27 14:04 1983 src/sys/sys/sys.c
 rw-r--r--1204/70  13515 Nov 13 11:56 1983 src/sys/sys/vminit.c
 rw-r--r--1204/70   5164 Aug 27 14:02 1983 src/sys/sys/sys2.c
 rw-r--r--1204/70   6010 Aug 18 21:52 1983 src/sys/sys/sys3.c
 rw-r--r--1204/70   4025 Nov  8 12:59 1983 src/sys/sys/sysent.c
 rw-r--r--1204/70   9823 Oct  9 17:43 1983 src/sys/sys/sys4.c
 rw-r--r--1204/70   7356 Nov 10 16:19 1982 src/sys/sys/paniclist
 rw-r--r--1204/70  15388 Nov  4 18:47 1983 src/sys/sys/vm.c
 rw-r--r--1204/70  15467 Sep  9 10:53 1983 src/sys/sys/vmsys.c
 rw-r--r--1204/70  12228 Sep  3 15:46 1983 src/sys/sys/vmmem.c
 rw-r--r--1204/70  19212 Nov 13 11:56 1983 src/sys/sys/vmswap.c
 rw-r--r--1204/70  10886 Aug 18 21:53 1983 src/sys/sys/vmsched.c
 rw-r--r--1204/70  15640 Nov  7 16:02 1983 src/sys/sys/vmtrap.c
 rw-r--r--1204/70  22221 Nov  8 23:02 1983 src/sys/sys/m16.s
 rw-r--r--1204/70  17028 Oct  4 12:17 1983 src/sys/sys/icu.s
 rw-r--r--1204/70  13503 Nov 13 11:57 1983 src/sys/sys/vmtext.c
 rw-r--r--1204/70   2858 Oct 11 14:17 1983 src/sys/sys/panics
 rwxr-xr-x1204/70      0 Dec 15 16:46 1983 src/sys/h/
 rw-r--r--1204/70   4809 Sep 16 12:24 1983 src/sys/h/makefile
 rw-r--r--1204/70    870 Aug 18 21:55 1983 src/sys/h/acct.h
 rw-r--r--1204/70    478 Aug 18 21:55 1983 src/sys/h/blt.h
 rw-r--r--1204/70   4227 Aug 18 21:55 1983 src/sys/h/bootop.h
 rw-r--r--1204/70   3981 Aug 18 21:55 1983 src/sys/h/buf.h
 rw-r--r--1204/70    489 Aug 18 21:55 1983 src/sys/h/callo.h
 rw-r--r--1204/70    584 Aug 18 21:55 1983 src/sys/h/callout.h
 rw-r--r--1204/70    341 Aug 18 21:55 1983 src/sys/h/clist.h
 rw-r--r--1204/70    892 Aug 18 21:55 1983 src/sys/h/conf.h
 rw-r--r--1204/70   7578 Aug 18 21:55 1983 src/sys/h/dcu.h
 rw-r--r--1204/70   5531 Sep  6 12:20 1983 src/sys/h/dcusize.h
 rw-r--r--1204/70   1000 Aug 18 21:55 1983 src/sys/h/devpm.h
 rw-r--r--1204/70   6336 Aug 18 21:55 1983 src/sys/h/devvm.h
 rw-r--r--1204/70    186 Aug 18 21:55 1983 src/sys/h/dir.h
 rw-r--r--1204/70    474 Aug 18 21:55 1983 src/sys/h/dk.h
 rw-r--r--1204/70    155 Aug 18 21:55 1983 src/sys/h/fblk.h
 rw-r--r--1204/70    784 Aug 18 21:55 1983 src/sys/h/file.h
 rw-r--r--1204/70   1231 Aug 18 21:55 1983 src/sys/h/filsys.h


                                 433010137-001                     Page 28 of 93


                                  GENIX SOURCE


 rw-r--r--1204/70    317 Aug 18 21:55 1983 src/sys/h/fpu.h
 rw-r--r--1204/70   5201 Aug 18 21:55 1983 src/sys/h/icu.h
 rw-r--r--1204/70    723 Aug 18 21:55 1983 src/sys/h/ino.h
 rw-r--r--1204/70   2157 Aug 18 21:55 1983 src/sys/h/inode.h
 rw-r--r--1204/70   4326 Aug 31 00:12 1983 src/sys/h/ioctl.h
 rw-r--r--1204/70   2034 Aug 18 21:55 1983 src/sys/h/mem.h
 rw-r--r--1204/70   9365 Jun 16 13:33 1982 src/sys/h/tags
 rw-r--r--1204/70   2317 Aug 29 11:45 1983 src/sys/h/mmu.h
 rw-r--r--1204/70    304 Aug 18 21:55 1983 src/sys/h/modtable.h
 rw-r--r--1204/70    513 Aug 18 21:55 1983 src/sys/h/mount.h
 rw-r--r--1204/70   2169 Aug 18 21:55 1983 src/sys/h/mtio.h
 rw-r--r--1204/70    291 Aug 18 21:55 1983 src/sys/h/msgbuf.h
 rw-r--r--1204/70   1387 Aug 18 21:55 1983 src/sys/h/panic.h
 rw-r--r--1204/70   4046 Aug 18 21:55 1983 src/sys/h/param.h
 rw-r--r--1204/70    184 Aug 18 21:55 1983 src/sys/h/pdma.h
 rw-r--r--1204/70   4180 Aug 30 23:15 1983 src/sys/h/proc.h
 rw-r--r--1204/70    931 Aug 18 21:55 1983 src/sys/h/psr.h
 rw-r--r--1204/70   4842 Sep  2 13:27 1983 src/sys/h/pte.h
 rw-r--r--1204/70    570 Aug 18 21:55 1983 src/sys/h/reboot.h
 rw-r--r--1204/70    443 Aug 18 21:55 1983 src/sys/h/reg.h
 rw-r--r--1204/70   1760 Aug 18 21:55 1983 src/sys/h/rs.h
 rw-r--r--1204/70   5581 Aug 18 21:55 1983 src/sys/h/sio.h
 rw-r--r--1204/70    871 Aug 18 21:55 1983 src/sys/h/stat.h
 rw-r--r--1204/70   2050 Aug 18 21:56 1983 src/sys/h/systm.h
 rw-r--r--1204/70   6262 Aug 18 21:56 1983 src/sys/h/tcu.h
 rw-r--r--1204/70    233 Aug 18 21:56 1983 src/sys/h/timeb.h
 rw-r--r--1204/70   1900 Aug 18 21:56 1983 src/sys/h/timer.h
 rw-r--r--1204/70    319 Aug 18 21:56 1983 src/sys/h/times.h
 rw-r--r--1204/70    390 Aug 18 21:56 1983 src/sys/h/trap.h
 rw-r--r--1204/70   4326 Aug 18 21:56 1983 src/sys/h/tty.h
 rw-r--r--1204/70   1131 Aug 18 21:56 1983 src/sys/h/types.h
 rw-r--r--1204/70   5115 Sep  9 15:45 1983 src/sys/h/user.h
 rw-r--r--1204/70   1370 Aug 18 21:56 1983 src/sys/h/vfont.h
 rw-r--r--1204/70    560 Aug 18 21:56 1983 src/sys/h/vlimit.h
 rw-r--r--1204/70   6629 Nov 13 11:57 1983 src/sys/h/vm.h
 rw-r--r--1204/70   1788 Aug 18 21:56 1983 src/sys/h/vmmeter.h
 rw-r--r--1204/70    208 Aug 18 21:56 1983 src/sys/h/vmsystm.h
 rw-r--r--1204/70    872 Aug 18 21:56 1983 src/sys/h/vmtune.h
 rw-r--r--1204/70    918 Aug 18 21:56 1983 src/sys/h/vtimes.h
 rw-r--r--1204/70   1095 Sep  2 16:31 1983 src/sys/h/vmswap.h
 rw-r--r--1204/70   2318 Sep 16 12:20 1983 src/sys/h/disk.h
 rwxr-xr-x1204/70      0 Dec 15 16:47 1983 src/sys/MESA/
 rw-r--r--1204/70  19858 Nov  9 17:23 1983 src/sys/MESA/makefile
 rw-r--r--1204/70      4 Nov 16 00:18 1983 src/sys/MESA/version
 rw-r--r--1204/70    794 Oct 13 13:52 1983 src/sys/MESA/stash.o
 rw-r--r--1204/70    680 Aug 15 18:39 1983 src/sys/makefile
 rwxr-xr-x1204/70      0 Dec 15 16:47 1983 src/sys/SYS16/
 rw-r--r--1204/70  19847 Nov 15 10:26 1983 src/sys/SYS16/makefile
 rw-r--r--1204/70      2 Dec  9 19:19 1983 src/sys/SYS16/version
 rwxr-xr-x1204/70   1788 Dec  5 15:41 1983 src/makefile
 rwxr-xr-x1204/70      0 Dec 15 16:56 1983 src/cmd/
 rwxr-xr-x1204/70   9502 Dec  9 08:06 1983 src/cmd/makefile
 rwxr-xr-x1204/70      0 Dec 15 16:47 1983 src/cmd/pcc/
 rw-r--r--1204/70  11804 Dec  9 21:28 1983 src/cmd/pcc/makefile
 r--r-----1204/70  16683 Sep 10 10:04 1983 src/cmd/pcc/mcc.c
 r--r-----1204/70   8217 Sep 10 10:05 1983 src/cmd/pcc/manifest
 r--r-----1204/70   8001 Sep 10 10:05 1983 src/cmd/pcc/mfile2
 r--r-----1204/70   1310 Sep 10 10:05 1983 src/cmd/pcc/mac2defs


                                 433010137-001                     Page 29 of 93


                                  GENIX SOURCE


 r--r-----1204/70   2889 Sep 10 10:05 1983 src/cmd/pcc/macdefs
 r--r-----1204/70  15125 Sep 10 10:05 1983 src/cmd/pcc/allo.c
 rwxr-xr-x1204/70    333 Nov 15 21:32 1983 src/cmd/pcc/:rofix
 r--r-----1204/70   6207 Sep 10 10:06 1983 src/cmd/pcc/mfile1
 r--r-----1204/70  23068 Sep 10 10:06 1983 src/cmd/pcc/cgram.y
 rwxr-xr-x1204/70    230 Dec  6 16:54 1983 src/cmd/pcc/:yyfix
 r--r-----1204/70  21232 Sep 10 10:09 1983 src/cmd/pcc/code.c
 r--r-----1204/70   7494 Sep 10 10:10 1983 src/cmd/pcc/common
 r--r-----1204/70   1096 Sep 10 10:10 1983 src/cmd/pcc/comm1.c
 r--r-----1204/70  16069 Sep 10 10:11 1983 src/cmd/pcc/local.c
 r--r-----1204/70  70109 Sep 10 10:12 1983 src/cmd/pcc/local2.c
 r--r-----1204/70  19453 Sep 10 10:14 1983 src/cmd/pcc/match.c
 r--r-----1204/70   4924 Sep 10 10:15 1983 src/cmd/pcc/optim.c
 r--r-----1204/70  52260 Sep 10 10:15 1983 src/cmd/pcc/order.c
 r--r-----1204/70  42483 Sep 10 10:17 1983 src/cmd/pcc/pftn.c
 r--r-----1204/70  47999 Sep 10 10:19 1983 src/cmd/pcc/reader.c
 r--r-----1204/70  25559 Sep 10 10:21 1983 src/cmd/pcc/scan.c
 r--r-----1204/70  46513 Sep 10 10:22 1983 src/cmd/pcc/table.c
 r--r-----1204/70  42812 Sep 10 10:23 1983 src/cmd/pcc/trees.c
 r--r-----1204/70   4113 Sep 10 10:25 1983 src/cmd/pcc/xdefs.c
 rw-r-----1204/70      0 Sep 13 00:18 1983 src/cmd/pcc/comm2.c
 rwxr-xr-x1204/70  11366 Jun 10 19:09 1983 src/cmd/ar.c
 rwxr-xr-x1204/70      0 Dec 15 16:47 1983 src/cmd/spell/
 r--r-----1204/70   1351 Aug 25 11:28 1983 src/cmd/spell/makefile
 r--r-----1204/70   1856 Jun 11 00:44 1983 src/cmd/spell/spell.h
 r--r-----1204/70    665 Jun 11 00:44 1983 src/cmd/spell/spell.sh
 r--r-----1204/70   8335 Jun 11 00:44 1983 src/cmd/spell/spell.c
 r--r-----1204/70   2982 Jun 11 00:46 1983 src/cmd/spell/american
 r--r-----1204/70   2527 Jun 11 00:46 1983 src/cmd/spell/local
 r--r-----1204/70   3209 Jun 11 00:46 1983 src/cmd/spell/british
 r--r-----1204/70   7875 Jun 11 00:46 1983 src/cmd/spell/stop
 r--r-----1204/70   1001 Jun 11 00:46 1983 src/cmd/spell/spellin.c
 r--r-----1204/70    890 Jun 11 00:47 1983 src/cmd/spell/spellout.c
 rwxr-xr-x1204/70   2407 Jun 10 19:12 1983 src/cmd/cat.c
 rwxr-xr-x1204/70   8652 Nov 15 19:46 1983 src/cmd/units.d
 rwxr-xr-x1204/70   2648 Jun 10 19:13 1983 src/cmd/chmod.c
 rwxr-xr-x1204/70  10674 Aug  3 18:03 1983 src/cmd/egrep.y
 rwxr-xr-x1204/70   1828 Jun 10 19:13 1983 src/cmd/cmp.c
 rwxr-xr-x1204/70   2600 Aug  5 13:37 1983 src/cmd/chsh.c
 rwxr-xr-x1204/70      0 Dec 15 16:47 1983 src/cmd/ls/
 r--r-----1204/70    689 Oct 31 15:15 1983 src/cmd/ls/makefile
 r--r-----1204/70   7381 Oct 31 15:15 1983 src/cmd/ls/ls.c
 r--r-----1204/70  29524 Oct 31 15:19 1983 src/cmd/ls/ucbls.c
 rwxr-xr-x1204/70      0 Dec 15 16:47 1983 src/cmd/sh/
 r--r-----1204/70   5067 Sep 26 18:32 1983 src/cmd/sh/makefile
 r--r-----1204/70    178 Jun 11 00:35 1983 src/cmd/sh/brkincr.h
 r--r-----1204/70   2419 Jun 11 00:35 1983 src/cmd/sh/ctype.h
 r--r-----1204/70   4737 Jun 11 00:35 1983 src/cmd/sh/defs.h
 r--r-----1204/70    222 Jun 11 00:35 1983 src/cmd/sh/dup.h
 r--r-----1204/70   1001 Jun 11 00:35 1983 src/cmd/sh/mac.h
 r--r-----1204/70   3336 Jun 11 00:35 1983 src/cmd/sh/mode.h
 r--r-----1204/70    438 Jun 11 00:35 1983 src/cmd/sh/name.h
 r--r-----1204/70   1720 Jun 11 00:35 1983 src/cmd/sh/stak.h
 r--r-----1204/70    888 Jun 11 00:35 1983 src/cmd/sh/sym.h
 r--r-----1204/70    231 Jun 11 00:35 1983 src/cmd/sh/timeout.h
 r--r-----1204/70    319 Jun 11 00:35 1983 src/cmd/sh/setbrk.c
 r--r-----1204/70    179 Jun 11 00:36 1983 src/cmd/sh/builtin.c
 r--r-----1204/70   2181 Jun 11 00:36 1983 src/cmd/sh/blok.c


                                 433010137-001                     Page 30 of 93


                                  GENIX SOURCE


 r--r-----1204/70   1349 Jun 11 00:36 1983 src/cmd/sh/stak.c
 r--r-----1204/70   6779 Jun 11 00:36 1983 src/cmd/sh/cmd.c
 r--r-----1204/70   1507 Jun 11 00:37 1983 src/cmd/sh/fault.c
 r--r-----1204/70   3539 Sep 26 14:26 1983 src/cmd/sh/main.c
 r--r-----1204/70   2557 Jun 11 00:38 1983 src/cmd/sh/word.c
 r--r-----1204/70    738 Jun 11 00:38 1983 src/cmd/sh/string.c
 r--r-----1204/70   4955 Jun 11 00:38 1983 src/cmd/sh/name.c
 r--r-----1204/70   2415 Jun 11 00:39 1983 src/cmd/sh/args.c
 r--r-----1204/70   9496 Jun 11 00:39 1983 src/cmd/sh/xec.c
 r--r-----1204/70   6457 Jun 11 00:40 1983 src/cmd/sh/service.c
 r--r-----1204/70   1243 Jun 11 00:41 1983 src/cmd/sh/error.c
 r--r-----1204/70   2010 Jun 11 00:41 1983 src/cmd/sh/io.c
 r--r-----1204/70   1226 Jun 11 00:41 1983 src/cmd/sh/print.c
 r--r-----1204/70   4411 Jun 11 00:41 1983 src/cmd/sh/macro.c
 r--r-----1204/70   3176 Jun 11 00:42 1983 src/cmd/sh/expand.c
 r--r-----1204/70   2057 Jun 11 00:42 1983 src/cmd/sh/ctype.c
 r--r-----1204/70   2818 Jun 11 00:43 1983 src/cmd/sh/msg.c
 rwxr-xr-x1204/70   2385 Jun 10 19:14 1983 src/cmd/cp.c
 rwxr-xr-x1204/70  21116 Oct  3 13:57 1983 src/cmd/ps.c
 rwxr-xr-x1204/70   2856 Jun 10 19:15 1983 src/cmd/date.c
 rwxr-xr-x1204/70  11958 Jun 10 19:15 1983 src/cmd/dd.c
 rwxr-xr-x1204/70   5142 Nov 13 15:53 1983 src/cmd/tcerr.c
 rwxr-xr-x1204/70   4332 Jun 20 18:27 1983 src/cmd/df.c
 rwxr-xr-x1204/70  33606 Nov 13 17:31 1983 src/cmd/nm.c
 rwxr-xr-x1204/70   3361 Jun 10 19:18 1983 src/cmd/du.c
 rwxr-xr-x1204/70    472 Jun 10 19:19 1983 src/cmd/echo.c
 rwxr-xr-x1204/70  11012 Jun 10 19:19 1983 src/cmd/expr.y
 rwxr-xr-x1204/70    675 Jun 10 19:21 1983 src/cmd/hostname.c
 rwxr-xr-x1204/70   1721 Jun 10 19:22 1983 src/cmd/kill.c
 rwxr-xr-x1204/70   1352 Jun 10 19:22 1983 src/cmd/ln.c
 rwxr-xr-x1204/70   7894 Aug 31 00:47 1983 src/cmd/login.c
 rwxr-xr-x1204/70  16408 Jun 10 19:24 1983 src/cmd/mail.c
 rwxr-xr-x1204/70   1373 Jun 10 19:27 1983 src/cmd/mkdir.c
 rwxr-xr-x1204/70   2480 Jun 28 15:48 1983 src/cmd/mt.c
 rwxr-xr-x1204/70   7262 Jun 10 19:28 1983 src/cmd/mv.c
 rwxr-xr-x1204/70    564 Jun 10 19:30 1983 src/cmd/nice.c
 rwxr-xr-x1204/70      0 Dec 15 16:47 1983 src/cmd/awk/
 rw-r-----1204/70   2662 Jun 30 15:54 1983 src/cmd/awk/EXPLAIN
 rw-r-----1204/70   2974 Jun 30 15:54 1983 src/cmd/awk/README
 rwxr-xr-x1204/70   4170 Dec  7 07:08 1983 src/cmd/awk/makefile
 r--r-----1204/70   3241 Aug 29 16:35 1983 src/cmd/awk/awk.def
 r--r-----1204/70   8502 Aug 29 16:35 1983 src/cmd/awk/awk.g.y
 r--r-----1204/70   4853 Aug 29 16:48 1983 src/cmd/awk/awk.lx.l
 r--r-----1204/70  11023 Aug 29 17:07 1983 src/cmd/awk/b.c
 r--r-----1204/70    684 Aug 29 18:04 1983 src/cmd/awk/freeze.c
 r--r-----1204/70   6252 Aug 29 18:05 1983 src/cmd/awk/lib.c
 rw-r-----1204/70  15094 Jun 30 15:54 1983 src/cmd/awk/test.a
 rw-r-----1204/70   3962 Jun 30 15:54 1983 src/cmd/awk/test2.c
 r--r-----1204/70   2829 Aug 29 18:11 1983 src/cmd/awk/main.c
 r--r-----1204/70   2471 Aug 29 18:13 1983 src/cmd/awk/parse.c
 r--r-----1204/70   2455 Aug 29 18:14 1983 src/cmd/awk/proc.c
 r--r-----1204/70   1563 Aug 29 18:14 1983 src/cmd/awk/token.c
 r--r-----1204/70    121 Aug 29 18:16 1983 src/cmd/awk/tokenscript
 r--r-----1204/70  14833 Aug 29 18:17 1983 src/cmd/awk/run.c
 r--r-----1204/70   5642 Aug 29 18:28 1983 src/cmd/awk/tran.c
 rwxr-xr-x1204/70   1255 Aug 31 00:56 1983 src/cmd/reset.c
 rwxr-xr-x1204/70   4445 Jun 10 19:30 1983 src/cmd/od.c
 rwxr-xr-x1204/70   3534 Jun 10 19:31 1983 src/cmd/passwd.c


                                 433010137-001                     Page 31 of 93


                                  GENIX SOURCE


 rwxr-xr-x1204/70   6424 Jun 10 19:32 1983 src/cmd/pr.c
 rwxr-xr-x1204/70    971 Aug 10 15:28 1983 src/cmd/lock.c
 rwxr-xr-x1204/70   1539 Jun 10 19:37 1983 src/cmd/pwd.c
 rwxr-xr-x1204/70      0 Dec 15 16:47 1983 src/cmd/cref/
 rwxr-xr-x1204/70   1999 Dec  8 14:53 1983 src/cmd/cref/makefile
 r--r-----1204/70    982 Sep 23 14:34 1983 src/cmd/cref/ccmn.h
 r--r-----1204/70    591 Sep 23 14:34 1983 src/cmd/cref/mcons.h
 r--r-----1204/70   6431 Sep 23 14:34 1983 src/cmd/cref/dr.c
 rw-r--r--1204/70    248 Dec 10 05:11 1983 src/cmd/cref/mkcref
 r--r-----1204/70    826 Sep 23 14:35 1983 src/cmd/cref/put.c
 r--r-----1204/70   6961 Sep 23 14:35 1983 src/cmd/cref/acts.c
 r--r-----1204/70   3507 Sep 23 14:36 1983 src/cmd/cref/crpost.c
 r--r-----1204/70   2712 Sep 23 14:36 1983 src/cmd/cref/upost.c
 rw-r-----1204/70   9064 Sep 23 14:36 1983 src/cmd/cref/crpost
 r--r-----1204/70   3106 Sep 23 14:37 1983 src/cmd/cref/mtab.c
 rw-r-----1204/70   9268 Sep 23 14:37 1983 src/cmd/cref/upost
 r--r-----1204/70    464 Sep 23 14:37 1983 src/cmd/cref/econs.h
 rw-r-----1204/70    120 Jun  3 21:53 1983 src/cmd/cref/.file
 r--r-----1204/70   3032 Sep 23 14:37 1983 src/cmd/cref/atable
 rw-r-----1204/70    768 Dec 10 05:11 1983 src/cmd/cref/atab
 r--r-----1204/70   3073 Sep 23 14:37 1983 src/cmd/cref/ctable
 rw-r-----1204/70    768 Dec 10 05:11 1983 src/cmd/cref/ctab
 r--r-----1204/70   2441 Sep 23 14:37 1983 src/cmd/cref/etable
 rw-r-----1204/70    768 Dec 10 05:11 1983 src/cmd/cref/etab
 r--r-----1204/70   2531 Sep 23 14:37 1983 src/cmd/cref/mign.c
 r--r-----1204/70    658 Sep 23 14:38 1983 src/cmd/cref/asym
 rw-r-----1204/70   5024 Dec 10 05:11 1983 src/cmd/cref/aign
 r--r-----1204/70    174 Sep 23 14:38 1983 src/cmd/cref/csym
 rw-r-----1204/70   5024 Dec 10 05:11 1983 src/cmd/cref/cign
 r--r-----1204/70    706 Sep 23 14:38 1983 src/cmd/cref/esym
 r--r-----1204/70    706 Sep 23 14:38 1983 src/cmd/cref/eign
 rwxr-xr-x1204/70   2877 Jun 10 19:37 1983 src/cmd/rm.c
 rwxr-xr-x1204/70   7870 Aug  9 15:02 1983 src/cmd/savecore.c
 rwxr-xr-x1204/70   2359 Jun 10 19:38 1983 src/cmd/rmdir.c
 rwxr-xr-x1204/70   2601 Jun 10 19:39 1983 src/cmd/rmail.c
 rwxr-xr-x1204/70      0 Dec 15 16:47 1983 src/cmd/ed/
 r--r-----1204/70    573 Aug 17 11:11 1983 src/cmd/ed/makefile
 r--r-----1204/70  25059 Jun 10 23:21 1983 src/cmd/ed/ed.c
 rwxr-xr-x1204/70   1053 Aug 26 13:56 1983 src/cmd/size.c
 rwxr-xr-x1204/70   2608 Jun 10 19:40 1983 src/cmd/strip.c
 rwxr-xr-x1204/70  11642 Aug 31 00:43 1983 src/cmd/stty.c
 rwxr-xr-x1204/70   5183 Aug 30 10:20 1983 src/cmd/ranlib.c
 rwxr-xr-x1204/70   2069 Jun 10 19:44 1983 src/cmd/su.c
 rwxr-xr-x1204/70    171 Jun 10 19:45 1983 src/cmd/sync.c
 rwxr-xr-x1204/70      0 Dec 15 16:47 1983 src/cmd/diff3/
 r--r-----1204/70    737 Aug 15 16:40 1983 src/cmd/diff3/makefile
 r--r-----1204/70   8129 Jun 10 23:16 1983 src/cmd/diff3/diff3.c
 r--r-----1204/70    388 Jun 14 16:13 1983 src/cmd/diff3/diff3.sh
 rwxr-xr-x1204/70      0 Dec 15 16:47 1983 src/cmd/man/
 r--r-----1204/70  12905 Aug 30 11:04 1983 src/cmd/man/man.c
 r--r-----1204/70    590 Aug 17 11:18 1983 src/cmd/man/makefile
 r--r-----1204/70    848 Jun 11 00:22 1983 src/cmd/man/man.sh
 rwxr-xr-x1204/70  17332 Aug 31 16:22 1983 src/cmd/tar.c
 rwxr-xr-x1204/70      0 Dec 15 16:47 1983 src/cmd/sed/
 r--r-----1204/70    620 Aug 25 11:27 1983 src/cmd/sed/makefile
 r--r-----1204/70  15304 Jun 11 00:33 1983 src/cmd/sed/sed0.c
 r--r-----1204/70   2428 Jun 11 00:33 1983 src/cmd/sed/sed.h
 r--r-----1204/70  10704 Jun 11 00:34 1983 src/cmd/sed/sed1.c


                                 433010137-001                     Page 32 of 93


                                  GENIX SOURCE


 rwxr-xr-x1204/70      0 Dec 15 16:47 1983 src/cmd/tset/
 rwxr-xr-x1204/70   1192 Dec  8 20:30 1983 src/cmd/tset/makefile
 r--r-----1204/70   1267 Jun 11 01:17 1983 src/cmd/tset/tset.dels.h
 r--r-----1204/70  28307 Jun 11 01:17 1983 src/cmd/tset/tset.c
 rwxr-xr-x1204/70      0 Dec 15 16:48 1983 src/cmd/tset/tabset/
 r--r--r--1204/70     93 Mar 17 15:34 1983 src/cmd/tset/tabset/3101
 r--r--r--1204/70    105 Mar 17 15:34 1983 src/cmd/tset/tabset/aa
 r--r--r--1204/70    164 Mar 17 15:34 1983 src/cmd/tset/tabset/beehive
 r--r--r--1204/70     88 Mar 17 15:34 1983 src/cmd/tset/tabset/diablo
 r--r--r--1204/70    135 Mar 17 15:34 1983 src/cmd/tset/tabset/std
 r--r--r--1204/70     95 Mar 17 15:34 1983 src/cmd/tset/tabset/stdcrt
 r--r--r--1204/70     57 Mar 17 15:34 1983 src/cmd/tset/tabset/teleray
 r--r--r--1204/70    159 Mar 17 15:34 1983 src/cmd/tset/tabset/vt100
 r--r--r--1204/70    164 Mar 17 15:34 1983 src/cmd/tset/tabset/xerox1720
 rwxr-xr-x1204/70   1536 Jun 10 19:49 1983 src/cmd/tee.c
 rwxr-xr-x1204/70   2618 Jun 10 19:49 1983 src/cmd/test.c
 rwxr-xr-x1204/70   1623 Jun 10 19:50 1983 src/cmd/time.c
 rwxr-xr-x1204/70   2727 Jun 10 19:50 1983 src/cmd/wall.c
 rwxr-xr-x1204/70   1498 Jun 10 19:51 1983 src/cmd/who.c
 rwxr-xr-x1204/70   3661 Jun 10 19:51 1983 src/cmd/write.c
 rwxr-xr-x1204/70   3978 Jun 10 19:52 1983 src/cmd/ac.c
 rwxr-xr-x1204/70    289 Jun 10 19:52 1983 src/cmd/accton.c
 rwxr-xr-x1204/70   2744 Jun 10 19:52 1983 src/cmd/catman.c
 rwxr-xr-x1204/70    908 Jun 10 19:53 1983 src/cmd/chgrp.c
 rwxr-xr-x1204/70    915 Jun 10 19:53 1983 src/cmd/chown.c
 rwxr-xr-x1204/70   1719 Jun 10 19:54 1983 src/cmd/clri.c
 rwxr-xr-x1204/70   3860 Jun 10 19:54 1983 src/cmd/cron.c
 rwxr-xr-x1204/70   5106 Jun 10 19:55 1983 src/cmd/dcheck.c
 rwxr-xr-x1204/70   1485 Jun 14 12:53 1983 src/cmd/uid.c
 rwxr-xr-x1204/70      0 Dec 15 16:48 1983 src/cmd/dump/
 r--r-----1204/70   1415 Aug 15 18:01 1983 src/cmd/dump/makefile
 r--r-----1204/70   3714 Jun 10 23:18 1983 src/cmd/dump/dump.h
 r--r-----1204/70   4741 Jun 10 23:18 1983 src/cmd/dump/dumpitime.c
 r--r-----1204/70   5777 Jun 10 23:18 1983 src/cmd/dump/dumptape.c
 r--r-----1204/70   4959 Jun 10 23:19 1983 src/cmd/dump/dumptrav.c
 r--r-----1204/70   1959 Jun 10 23:19 1983 src/cmd/dump/unctime.c
 r--r-----1204/70   5748 Jun 10 23:19 1983 src/cmd/dump/dumpmain.c
 r--r-----1204/70   9104 Jun 10 23:20 1983 src/cmd/dump/dumpoptr.c
 rwxr-xr-x1204/70  37476 Aug 18 17:01 1983 src/cmd/fsck.c
 rwxr-xr-x1204/70  12085 Jul 29 11:05 1983 src/cmd/getty.c
 rwxr-xr-x1204/70   2453 Jun 10 19:59 1983 src/cmd/halt.c
 rwxr-xr-x1204/70      0 Dec 15 16:48 1983 src/cmd/msgs/
 r--r-----1204/70    555 Aug 17 11:18 1983 src/cmd/msgs/makefile
 r--r-----1204/70    345 Jun 11 00:22 1983 src/cmd/msgs/msgs.h
 r--r-----1204/70  13976 Jun 11 00:23 1983 src/cmd/msgs/msgs.c
 rwxr-xr-x1204/70   9646 Jun 13 14:25 1983 src/cmd/icheck.c
 rwxr-xr-x1204/70   6814 Aug 25 14:34 1983 src/cmd/init.c
 rwxr-xr-x1204/70  10700 Sep 26 17:29 1983 src/cmd/mkfs.c
 rwxr-xr-x1204/70      0 Dec 15 16:48 1983 src/cmd/dpy/
 r--r-----1204/70    672 Sep 29 18:20 1983 src/cmd/dpy/makefile
 r--r-----1204/70    949 Jun 10 23:16 1983 src/cmd/dpy/scan.h
 r--r-----1204/70   9090 Sep 29 18:22 1983 src/cmd/dpy/dpy.c
 r--r-----1204/70   5270 Jun 10 23:17 1983 src/cmd/dpy/scan.c
 rwxr-xr-x1204/70      0 Dec 15 16:48 1983 src/cmd/xsend/
 rwxr-xr-x1204/70    875 Dec  8 21:05 1983 src/cmd/xsend/makefile
 r--r-----1204/70    337 Jun 11 01:52 1983 src/cmd/xsend/xmail.h
 r--r-----1204/70   2084 Jun 11 01:52 1983 src/cmd/xsend/lib.c
 r--r-----1204/70    804 Jun 11 01:53 1983 src/cmd/xsend/enroll.c


                                 433010137-001                     Page 33 of 93


                                  GENIX SOURCE


 r--r-----1204/70   2653 Jun 11 01:53 1983 src/cmd/xsend/xsend.c
 r--r-----1204/70   2561 Jun 11 01:54 1983 src/cmd/xsend/xget.c
 r--r-----1204/70     31 Nov 16 18:48 1983 src/cmd/xsend/notice
 rwxr-xr-x1204/70      0 Dec 15 16:48 1983 src/cmd/calendar/
 r--r-----1204/70    752 Aug 15 15:19 1983 src/cmd/calendar/makefile
 r--r-----1204/70    983 Jun 10 22:40 1983 src/cmd/calendar/calendar.c
 r--r-----1204/70    625 Jun 14 16:08 1983 src/cmd/calendar/calendar.sh
 rwxr-xr-x1204/70    714 Jun 10 20:03 1983 src/cmd/mknod.c
 rwxr-xr-x1204/70   2511 Jun 10 20:03 1983 src/cmd/mount.c
 rwxr-xr-x1204/70   6761 Jun 10 20:03 1983 src/cmd/ncheck.c
 rwxr-xr-x1204/70  24420 Oct 14 14:34 1983 src/cmd/restor.c
 rwxr-xr-x1204/70      0 Dec 15 16:48 1983 src/cmd/diff/
 r--r-----1204/70    965 Aug 17 11:09 1983 src/cmd/diff/makefile
 r--r-----1204/70   1967 Jun 10 23:12 1983 src/cmd/diff/diff.h
 r--r-----1204/70   3165 Jun 10 23:12 1983 src/cmd/diff/diff.c
 r--r-----1204/70   7783 Jun 10 23:13 1983 src/cmd/diff/diffdir.c
 r--r-----1204/70  15464 Jun 10 23:13 1983 src/cmd/diff/diffreg.c
 r--r-----1204/70   4276 Jun 10 23:15 1983 src/cmd/diff/diffh.c
 rwxr-xr-x1204/70   7242 Jun 10 20:04 1983 src/cmd/quot.c
 rwxr-xr-x1204/70   2310 Jun 10 20:05 1983 src/cmd/reboot.c
 rwxr-xr-x1204/70      0 Dec 15 16:48 1983 src/cmd/dc/
 r--r-----1204/70    532 Aug 15 16:44 1983 src/cmd/dc/makefile
 r--r-----1204/70   3349 Jun 10 22:59 1983 src/cmd/dc/dc.h
 r--r-----1204/70  31697 Jun 10 22:59 1983 src/cmd/dc/dc.c
 rwxr-xr-x1204/70    797 Sep 28 16:14 1983 src/cmd/badsect.c
 rwxr-xr-x1204/70   1875 Jun 10 20:06 1983 src/cmd/renice.c
 rwxr-xr-x1204/70   7911 Oct 14 14:42 1983 src/cmd/dumpdir.c
 rwxr-xr-x1204/70  21803 Jun 10 20:08 1983 src/cmd/sa.c
 rwxr-xr-x1204/70  19567 Aug 10 16:51 1983 src/cmd/cu.c
 rwxr-xr-x1204/70      0 Dec 15 16:48 1983 src/cmd/compact/
 r--r-----1204/70    924 Aug 15 16:02 1983 src/cmd/compact/makefile
 r--r-----1204/70    987 Jun 10 22:40 1983 src/cmd/compact/compact.h
 r--r-----1204/70   6201 Jun 10 22:40 1983 src/cmd/compact/compact.c
 r--r-----1204/70   3048 Jun 10 22:41 1983 src/cmd/compact/tree.c
 r--r-----1204/70   4557 Jun 10 22:41 1983 src/cmd/compact/uncompact.c
 r--r-----1204/70    168 Jun 10 22:42 1983 src/cmd/compact/ccat.sh
 rwxr-xr-x1204/70   7470 Jun 10 20:11 1983 src/cmd/shutdown.c
 rwxr-xr-x1204/70   2176 Jun 10 20:12 1983 src/cmd/umount.c
 rwxr-xr-x1204/70   2412 Oct 31 18:04 1983 src/cmd/uvers.c
 rwxr-xr-x1204/70    587 Jun 10 20:13 1983 src/cmd/update.c
 rwxr-xr-x1204/70   7269 Sep  7 16:13 1983 src/cmd/vmstat.c
 rwxr-xr-x1204/70      0 Dec 15 16:48 1983 src/cmd/troff/
 r--r-----1204/70   1690 Jul 11 16:29 1983 src/cmd/troff/makefile
 rwxr-xr-x1204/70      0 Dec 15 16:48 1983 src/cmd/troff/font/
 r--r-----1204/70   1298 Jun 10 17:25 1983 src/cmd/troff/font/makefile
 r--r-----1204/70    497 Jun 11 01:16 1983 src/cmd/troff/font/chars.c
 r--r-----1204/70   2009 Jun 11 01:16 1983 src/cmd/troff/font/ftB.c
 r--r-----1204/70   2018 Jun 11 01:16 1983 src/cmd/troff/font/ftBC.c
 r--r-----1204/70    291 Jun 11 01:16 1983 src/cmd/troff/font/ftBC.n
 r--r-----1204/70   2054 Jun 11 01:16 1983 src/cmd/troff/font/ftC.c
 r--r-----1204/70    290 Jun 11 01:16 1983 src/cmd/troff/font/ftC.n
 r--r-----1204/70   2016 Jun 11 01:16 1983 src/cmd/troff/font/ftCE.c
 r--r-----1204/70   2088 Jun 11 01:16 1983 src/cmd/troff/font/ftCI.c
 r--r-----1204/70   2031 Jun 11 01:16 1983 src/cmd/troff/font/ftCK.c
 r--r-----1204/70   2089 Jun 11 01:16 1983 src/cmd/troff/font/ftCS.c
 r--r-----1204/70   2033 Jun 11 01:16 1983 src/cmd/troff/font/ftCW.c
 r--r-----1204/70    306 Jun 11 01:16 1983 src/cmd/troff/font/ftCW.n
 r--r-----1204/70   2009 Jun 11 01:16 1983 src/cmd/troff/font/ftG.c


                                 433010137-001                     Page 34 of 93


                                  GENIX SOURCE


 r--r-----1204/70   2004 Jun 11 01:16 1983 src/cmd/troff/font/ftGI.c
 r--r-----1204/70   2031 Jun 11 01:16 1983 src/cmd/troff/font/ftGM.c
 r--r-----1204/70   2014 Jun 11 01:16 1983 src/cmd/troff/font/ftGR.c
 r--r-----1204/70    285 Jun 11 01:16 1983 src/cmd/troff/font/ftGR.n
 r--r-----1204/70   2007 Jun 11 01:16 1983 src/cmd/troff/font/ftI.c
 r--r-----1204/70   2032 Jun 11 01:16 1983 src/cmd/troff/font/ftL.c
 r--r-----1204/70   2042 Jun 11 01:16 1983 src/cmd/troff/font/ftLI.c
 r--r-----1204/70   2308 Jun 11 01:16 1983 src/cmd/troff/font/ftPA.c
 r--r-----1204/70   2335 Jun 11 01:16 1983 src/cmd/troff/font/ftPB.c
 r--r-----1204/70   2325 Jun 11 01:16 1983 src/cmd/troff/font/ftPI.c
 r--r-----1204/70   2009 Jun 11 01:16 1983 src/cmd/troff/font/ftR.c
 r--r-----1204/70    303 Jun 11 01:16 1983 src/cmd/troff/font/ftR.n
 r--r-----1204/70   2660 Jun 11 01:16 1983 src/cmd/troff/font/ftS.c
 r--r-----1204/70   2039 Jun 11 01:16 1983 src/cmd/troff/font/ftSB.c
 r--r-----1204/70   2046 Jun 11 01:16 1983 src/cmd/troff/font/ftSI.c
 r--r-----1204/70   2040 Jun 11 01:16 1983 src/cmd/troff/font/ftSM.c
 r--r-----1204/70   2030 Jun 11 01:16 1983 src/cmd/troff/font/ftUD.c
 r--r-----1204/70   2252 Jun 11 01:16 1983 src/cmd/troff/font/ftXM.c
 r--r-----1204/70   1084 Jun 11 01:16 1983 src/cmd/troff/font/makefonts.c
 r--r-----1204/70   2029 Jun 11 01:17 1983 src/cmd/troff/font/mkfont.c
 r--r-----1204/70   5494 Jun 11 01:17 1983 src/cmd/troff/font/mkfont1.c
 rwxr-xr-x1204/70      0 Dec 15 16:48 1983 src/cmd/troff/term/
 r--r-----1204/70   4977 Jun 11 01:13 1983 src/cmd/troff/term/code.300
 r--r-----1204/70    643 Jun 11 01:13 1983 src/cmd/troff/term/tab300.c
 r--r-----1204/70    665 Jun 11 01:13 1983 src/cmd/troff/term/tab300s.c
 r--r-----1204/70    665 Jun 11 01:13 1983 src/cmd/troff/term/tab300S.c
 r--r-----1204/70    646 Jun 11 01:13 1983 src/cmd/troff/term/tab300X.c
 r--r-----1204/70   5108 Jun 11 01:13 1983 src/cmd/troff/term/tab37.c
 r--r-----1204/70    657 Jun 11 01:13 1983 src/cmd/troff/term/tab450.c
 r--r-----1204/70    689 Jun 11 01:13 1983 src/cmd/troff/term/tab450-12.c
 r--r-----1204/70    746 Jun 11 01:14 1983 src/cmd/troff/term/tab450-12-8.c
 r--r-----1204/70    661 Jun 11 01:14 1983 src/cmd/troff/term/tab450X.c
 r--r-----1204/70   4818 Jun 11 01:14 1983 src/cmd/troff/term/tablpr.c
 r--r-----1204/70   4786 Jun 11 01:14 1983 src/cmd/troff/term/tabtn300.c
 r--r-----1204/70   5140 Jun 11 01:14 1983 src/cmd/troff/term/tabdm.c
 r--r-----1204/70   5164 Jun 11 01:14 1983 src/cmd/troff/term/tabh2.c
 r--r-----1204/70   4832 Jun 11 01:14 1983 src/cmd/troff/term/tablpr2.c
 r--r-----1204/70    714 Jun 11 01:14 1983 src/cmd/troff/term/tabx1700.c
 r--r-----1204/70   3310 Jun 11 01:14 1983 src/cmd/troff/term/maketerms.c
 r--r-----1204/70   1034 Jun 10 17:24 1983 src/cmd/troff/term/makefile
 r--r-----1204/70    206 Jun 11 00:55 1983 src/cmd/troff/d.h
 r--r-----1204/70    250 Jun 11 00:55 1983 src/cmd/troff/str.h
 r--r-----1204/70   3677 Jun 11 00:55 1983 src/cmd/troff/tdef.h
 r--r-----1204/70    499 Jun 11 00:55 1983 src/cmd/troff/tw.h
 r--r-----1204/70    189 Jun 11 00:55 1983 src/cmd/troff/v.h
 r--r-----1204/70   5390 Jun 11 00:55 1983 src/cmd/troff/ni.c
 r--r-----1204/70   1731 Jun 11 00:55 1983 src/cmd/troff/nii.c
 r--r-----1204/70  16423 Oct 19 16:13 1983 src/cmd/troff/n1.c
 r--r-----1204/70   5142 Jun 11 00:57 1983 src/cmd/troff/n2.c
 r--r-----1204/70  11126 Jun 11 00:57 1983 src/cmd/troff/n3.c
 r--r-----1204/70   9578 Oct 19 16:13 1983 src/cmd/troff/n4.c
 r--r-----1204/70  11981 Jun 11 00:59 1983 src/cmd/troff/n5.c
 r--r-----1204/70  10192 Jun 11 01:00 1983 src/cmd/troff/t6.c
 r--r-----1204/70  11572 Jun 11 01:01 1983 src/cmd/troff/n7.c
 r--r-----1204/70   4227 Jun 11 01:01 1983 src/cmd/troff/n8.c
 r--r-----1204/70   5757 Jun 11 01:02 1983 src/cmd/troff/n9.c
 r--r-----1204/70   5348 Jun 11 01:03 1983 src/cmd/troff/t10.c
 r--r-----1204/70   7349 Jun 11 01:03 1983 src/cmd/troff/hytab.c


                                 433010137-001                     Page 35 of 93


                                  GENIX SOURCE


 r--r-----1204/70  19274 Jun 11 01:03 1983 src/cmd/troff/suftab.c
 r--r-----1204/70  16053 Jun 11 01:04 1983 src/cmd/troff/tab3.c
 r--r-----1204/70   4539 Jun 11 01:09 1983 src/cmd/troff/n6.c
 r--r-----1204/70   4496 Jun 11 01:11 1983 src/cmd/troff/n10.c
 r--r-----1204/70   4536 Jun 11 01:13 1983 src/cmd/troff/ntab.c
 r--r-----1204/70   8837 Oct 19 15:58 1983 src/cmd/troff/n41.4.c
 rwxr-xr-x1204/70   6364 Oct  5 19:17 1983 src/cmd/at.c
 rwxr-xr-x1204/70   1341 Jun 10 20:14 1983 src/cmd/basename.c
 rwxr-xr-x1204/70  12582 Jun 10 20:14 1983 src/cmd/bc.y
 rwxr-xr-x1204/70      0 Dec 15 16:49 1983 src/cmd/ex/
 rwxr-xr-x1204/70   5425 Aug 17 11:16 1983 src/cmd/ex/makefile
 rw-r-----1204/70   2212 Sep 29 15:54 1982 src/cmd/ex/READ_ME
 rwxr-xr-x1204/70  12511 Sep 27 14:16 1983 src/cmd/ex/ex.h
 rwxr-xr-x1204/70    945 Sep 27 14:18 1983 src/cmd/ex/ex_argv.h
 rwxr-xr-x1204/70   1976 Sep 27 14:18 1983 src/cmd/ex/ex_re.h
 rwxr-xr-x1204/70   4787 Sep 27 14:18 1983 src/cmd/ex/ex_temp.h
 rwxr-xr-x1204/70   7351 Sep 27 14:18 1983 src/cmd/ex/ex_tty.h
 rwxr-xr-x1204/70   3732 Sep 27 14:18 1983 src/cmd/ex/ex_tune.h
 rwxr-xr-x1204/70   1236 Sep 27 14:18 1983 src/cmd/ex/ex_vars.h
 rwxr-xr-x1204/70   9276 Sep 27 14:18 1983 src/cmd/ex/ex_vis.h
 rwxr-xr-x1204/70   8856 Sep 27 14:18 1983 src/cmd/ex/ex.c
 rwxr-xr-x1204/70   5455 Sep 27 14:18 1983 src/cmd/ex/ex_addr.c
 rwxr-xr-x1204/70    353 Aug 16 18:53 1983 src/cmd/ex/bcopy.s
 rwxr-xr-x1204/70  13977 Sep 27 14:18 1983 src/cmd/ex/ex_cmds.c
 rwxr-xr-x1204/70   8982 Sep 27 14:18 1983 src/cmd/ex/ex_cmds2.c
 rwxr-xr-x1204/70  24513 Sep 27 14:18 1983 src/cmd/ex/ex_cmdsub.c
 r--r-----1204/70   2649 Jun 16 19:37 1983 src/cmd/ex/ex_data.c
 rwxr-xr-x1204/70   4403 Sep 27 14:18 1983 src/cmd/ex/ex_get.c
 rwxr-xr-x1204/70  14930 Sep 27 14:18 1983 src/cmd/ex/ex_io.c
 r--r-----1204/70  19272 Sep 13 20:01 1983 src/cmd/ex/ex_put.c
 rwxr-xr-x1204/70  16483 Sep 27 14:18 1983 src/cmd/ex/ex_re.c
 rwxr-xr-x1204/70   4833 Sep 27 14:18 1983 src/cmd/ex/ex_set.c
 rwxr-xr-x1204/70  13851 Sep 27 14:18 1983 src/cmd/ex/ex_subr.c
 r--r-----1204/70  14445 Sep 27 13:48 1983 src/cmd/ex/ex_temp.c
 rwxr-xr-x1204/70   5296 Sep 27 14:18 1983 src/cmd/ex/ex_tty.c
 rwxr-xr-x1204/70   6019 Sep 27 14:18 1983 src/cmd/ex/ex_unix.c
 rwxr-xr-x1204/70   7999 Sep 27 14:18 1983 src/cmd/ex/ex_v.c
 rwxr-xr-x1204/70  23141 Sep 27 14:18 1983 src/cmd/ex/ex_vadj.c
 rwxr-xr-x1204/70  13182 Sep 27 14:18 1983 src/cmd/ex/ex_vget.c
 rwxr-xr-x1204/70  27389 Sep 27 14:18 1983 src/cmd/ex/ex_vmain.c
 rwxr-xr-x1204/70  14944 Sep 27 14:18 1983 src/cmd/ex/ex_voper.c
 rwxr-xr-x1204/70  18712 Sep 27 14:18 1983 src/cmd/ex/ex_vops.c
 rwxr-xr-x1204/70  18628 Sep 27 14:18 1983 src/cmd/ex/ex_vops2.c
 rwxr-xr-x1204/70   9892 Sep 27 14:18 1983 src/cmd/ex/ex_vops3.c
 rwxr-xr-x1204/70  30445 Sep 27 14:18 1983 src/cmd/ex/ex_vput.c
 rwxr-xr-x1204/70   8225 Sep 27 14:18 1983 src/cmd/ex/ex_vwind.c
 rwxr-xr-x1204/70   7754 Sep 27 14:18 1983 src/cmd/ex/printf.c
 rwxr-xr-x1204/70     34 Jun 10 23:56 1983 src/cmd/ex/rofix.ed
 r--r-----1204/70  15645 Sep 27 18:42 1983 src/cmd/ex/exrecover.c
 r--r-----1204/70   7607 Jun 16 19:40 1983 src/cmd/ex/expreserve.c
 rw-r-----1204/70    490 Sep 27 14:18 1983 src/cmd/ex/conv.c
 rwxr-xr-x1204/70   2806 Jun 10 20:17 1983 src/cmd/cal.c
 rwxr-xr-x1204/70   6112 Jun 10 20:17 1983 src/cmd/cb.c
 rwxr-xr-x1204/70   2073 Jun 10 20:18 1983 src/cmd/checkeq.c
 rwxr-xr-x1204/70   4339 Jun 10 20:18 1983 src/cmd/col.c
 rwxr-xr-x1204/70   2334 Jun 10 20:19 1983 src/cmd/comm.c
 rwxr-xr-x1204/70   1682 Jun 10 20:19 1983 src/cmd/crypt.c
 rwxr-xr-x1204/70      0 Dec 15 16:49 1983 src/cmd/csh/


                                 433010137-001                     Page 36 of 93


                                  GENIX SOURCE


 r--r-----1204/70  10267 Sep 16 14:26 1983 src/cmd/csh/makefile
 r--r-----1204/70    437 Jun 10 22:42 1983 src/cmd/csh/sh.dir.h
 r--r-----1204/70  12104 Jun 10 22:42 1983 src/cmd/csh/sh.h
 r--r-----1204/70   1731 Jun 10 22:42 1983 src/cmd/csh/sh.local.h
 r--r-----1204/70   3063 Jun 10 22:42 1983 src/cmd/csh/sh.proc.h
 r--r-----1204/70  18582 Jun 10 22:42 1983 src/cmd/csh/sh.c
 r--r-----1204/70  13287 Jun 10 22:43 1983 src/cmd/csh/sh.dol.c
 r--r-----1204/70   2840 Jun 10 22:44 1983 src/cmd/csh/sh.err.c
 r--r-----1204/70   6257 Jun 10 22:44 1983 src/cmd/csh/sh.exec.c
 r--r-----1204/70   9170 Jun 10 22:45 1983 src/cmd/csh/sh.exp.c
 r--r-----1204/70  19287 Jun 10 22:46 1983 src/cmd/csh/sh.func.c
 r--r-----1204/70  12316 Jun 10 22:47 1983 src/cmd/csh/sh.glob.c
 r--r-----1204/70   1892 Jun 10 22:48 1983 src/cmd/csh/sh.hist.c
 r--r-----1204/70  20870 Jun 10 22:48 1983 src/cmd/csh/sh.lex.c
 r--r-----1204/70   4907 Jun 10 22:50 1983 src/cmd/csh/sh.misc.c
 r--r-----1204/70   4852 Jun 10 22:56 1983 src/cmd/csh/alloc.c
 r--r-----1204/70  10203 Jun 10 22:50 1983 src/cmd/csh/sh.parse.c
 r--r-----1204/70   1642 Jun 10 22:51 1983 src/cmd/csh/sh.print.c
 r--r-----1204/70   8405 Jun 10 22:52 1983 src/cmd/csh/sh.sem.c
 r--r-----1204/70   8178 Jun 10 22:52 1983 src/cmd/csh/sh.set.c
 r--r-----1204/70  23152 Jun 10 22:53 1983 src/cmd/csh/sh.proc.c
 r--r-----1204/70   6960 Jun 10 22:55 1983 src/cmd/csh/sh.dir.c
 r--r-----1204/70   3155 Jun 10 22:55 1983 src/cmd/csh/sh.time.c
 r--r-----1204/70   4782 Jun 10 22:56 1983 src/cmd/csh/sh.init.c
 r--r-----1204/70   7626 Jun 10 22:56 1983 src/cmd/csh/printf.c
 r--r-----1204/70     34 Jun 10 22:57 1983 src/cmd/csh/rofix.ed
 rwxr-xr-x1204/70   6293 Jun 10 20:21 1983 src/cmd/fgrep.c
 rwxr-xr-x1204/70   7335 Jun 10 20:22 1983 src/cmd/file.c
 rwxr-xr-x1204/70  14760 Jun 10 20:23 1983 src/cmd/find.c
 rwxr-xr-x1204/70   2282 Jun 10 20:24 1983 src/cmd/look.c
 rwxr-xr-x1204/70    954 Jun 10 20:25 1983 src/cmd/mesg.c
 rwxr-xr-x1204/70   1199 Jun 10 20:25 1983 src/cmd/newgrp.c
 rwxr-xr-x1204/70   2228 Sep 28 16:15 1983 src/cmd/dmesg.c
 rwxr-xr-x1204/70  12822 Jun 10 20:27 1983 src/cmd/prof.c
 rwxr-xr-x1204/70   9773 Jun 10 20:28 1983 src/cmd/ptx.c
 rwxr-xr-x1204/70    122 Nov 15 18:09 1983 src/cmd/false.sh
 rwxr-xr-x1204/70      0 Dec 15 16:49 1983 src/cmd/termcap/
 r--r-----1204/70    785 Jul  5 15:56 1983 src/cmd/termcap/makefile
 rw-r-----1204/70   2123 May 10 14:27 1983 src/cmd/termcap/READ_ME
 r--r-----1204/70  41053 Jun 16 17:18 1983 src/cmd/termcap/termcap.src
 r--r-----1204/70    178 Jun 16 19:01 1983 src/cmd/termcap/reorder
 rwxr-xr-x1204/70      0 Dec 15 16:49 1983 src/cmd/termcap/tabset/
 r--r-----1204/70    412 Jun 10 17:24 1983 src/cmd/termcap/tabset/makefile
 r--r-----1204/70     93 Jun 14 19:00 1983 src/cmd/termcap/tabset/3101
 r--r-----1204/70    164 Jun 14 19:00 1983 src/cmd/termcap/tabset/beehive
 r--r-----1204/70     88 Jun 14 19:00 1983 src/cmd/termcap/tabset/diablo
 r--r-----1204/70    161 Jun 14 19:00 1983 src/cmd/termcap/tabset/direct
 r--r-----1204/70    135 Jun 14 19:00 1983 src/cmd/termcap/tabset/std
 r--r-----1204/70     57 Jun 14 19:00 1983 src/cmd/termcap/tabset/teleray
 r--r-----1204/70    159 Jun 14 19:00 1983 src/cmd/termcap/tabset/vt100
 r--r-----1204/70    164 Jun 14 19:00 1983 src/cmd/termcap/tabset/xerox1720
 rwxr-xr-x1204/70    766 Jun 10 20:30 1983 src/cmd/rev.c
 rwxr-xr-x1204/70  23744 Nov 18 17:16 1983 src/cmd/setroot.csh
 rwxr-xr-x1204/70    409 Jun 10 20:30 1983 src/cmd/sleep.c
 rwxr-xr-x1204/70  16066 Jun 10 20:31 1983 src/cmd/sort.c
 rwxr-xr-x1204/70    113 Nov 15 18:29 1983 src/cmd/true.sh
 rwxr-xr-x1204/70   1342 Jun 10 20:32 1983 src/cmd/split.c
 rwxr-xr-x1204/70    260 Nov 15 18:18 1983 src/cmd/nohup.sh


                                 433010137-001                     Page 37 of 93


                                  GENIX SOURCE


 rwxr-xr-x1204/70    967 Jun 10 20:32 1983 src/cmd/sum.c
 rwxr-xr-x1204/70    570 Nov 15 18:46 1983 src/cmd/mkl+s.sh
 rwxr-xr-x1204/70   2793 Jun 10 20:33 1983 src/cmd/tabs.c
 rwxr-xr-x1204/70   3199 Aug 17 09:59 1983 src/cmd/nburn.c
 rwxr-xr-x1204/70   3691 Jun 10 20:33 1983 src/cmd/tail.c
 rwxr-xr-x1204/70    913 Nov 15 18:48 1983 src/cmd/rc.sh
 rwxr-xr-x1204/70    988 Jun 10 20:34 1983 src/cmd/touch.c
 rwxr-xr-x1204/70    338 Nov 15 18:55 1983 src/cmd/vipw.sh
 rwxr-xr-x1204/70   2291 Jun 10 20:34 1983 src/cmd/tr.c
 rwxr-xr-x1204/70   4186 Jun 10 20:34 1983 src/cmd/tsort.c
 rwxr-xr-x1204/70   3759 Nov  3 11:08 1983 src/cmd/stat.c
 rwxr-xr-x1204/70    368 Jun 10 20:35 1983 src/cmd/tty.c
 rwxr-xr-x1204/70   2041 Jun 10 20:35 1983 src/cmd/uniq.c
 rwxr-xr-x1204/70   6369 Sep 20 09:46 1983 src/cmd/units.c
 rwxr-xr-x1204/70   3495 Jun 10 20:36 1983 src/cmd/wc.c
 rwxr-xr-x1204/70    369 Nov 15 18:47 1983 src/cmd/newsym.csh
 rwxr-xr-x1204/70   2211 Nov 15 19:09 1983 src/cmd/minstall.sh
 rwxr-xr-x1204/70  12380 Jun 10 20:37 1983 src/cmd/checknr.c
 rwxr-xr-x1204/70   2108 Jun 10 20:38 1983 src/cmd/chfn.c
 rwxr-xr-x1204/70    837 Jun 10 20:39 1983 src/cmd/clear.c
 rwxr-xr-x1204/70      0 Dec 15 16:49 1983 src/cmd/make/
 r--r-----1204/70   1364 Aug 17 11:17 1983 src/cmd/make/makefile
 r--r-----1204/70   2329 Jun 11 00:18 1983 src/cmd/make/defs.h
 r--r-----1204/70   3816 Jun 11 00:18 1983 src/cmd/make/ident.c
 r--r-----1204/70   6537 Jun 11 00:18 1983 src/cmd/make/main.c
 r--r-----1204/70   6015 Jun 11 00:19 1983 src/cmd/make/doname.c
 r--r-----1204/70   4692 Jun 11 00:19 1983 src/cmd/make/misc.c
 r--r-----1204/70   8814 Jun 11 00:20 1983 src/cmd/make/files.c
 r--r-----1204/70   2664 Jun 11 00:20 1983 src/cmd/make/dosys.c
 r--r-----1204/70   5217 Jun 11 00:21 1983 src/cmd/make/gram.y
 rwxr-xr-x1204/70   3980 Jun 10 20:40 1983 src/cmd/colcrt.c
 rwxr-xr-x1204/70    149 Nov 15 19:20 1983 src/cmd/apropos.sh
 rwxr-xr-x1204/70   1218 Jun 10 20:40 1983 src/cmd/colrm.c
 rwxr-xr-x1204/70  13324 Jun 10 20:40 1983 src/cmd/ctags.c
 rwxr-xr-x1204/70   1886 Jun 10 20:42 1983 src/cmd/expand.c
 rwxr-xr-x1204/70  31445 Jun 10 20:42 1983 src/cmd/finger.c
 rwxr-xr-x1204/70      0 Dec 15 16:50 1983 src/cmd/sccs/
 rwxr-xr-x1204/70      0 Dec 15 16:49 1983 src/cmd/sccs/hdr/
 rwxr-xr-x1204/70    316 Dec  8 11:11 1983 src/cmd/sccs/hdr/makefile
 r--r-----1204/70   4480 Nov 16 01:28 1983 src/cmd/sccs/hdr/defines.h
 r--r-----1204/70    874 Nov 16 01:28 1983 src/cmd/sccs/hdr/had.h
 rwxr-xr-x1204/70      0 Dec 15 16:50 1983 src/cmd/sccs/src/
 rwxr-xr-x1204/70    466 Dec  8 11:09 1983 src/cmd/sccs/src/makefile
 r--r-----1204/70  20702 Nov 16 01:28 1983 src/cmd/sccs/src/admin.c
 r--r-----1204/70   6953 Nov 16 01:28 1983 src/cmd/sccs/src/bdiff.c
 r--r-----1204/70   5860 Nov 16 01:28 1983 src/cmd/sccs/src/comb.c
 r--r-----1204/70  14603 Nov 16 01:28 1983 src/cmd/sccs/src/delta.c
 r--r-----1204/70  17805 Nov 16 01:28 1983 src/cmd/sccs/src/get.c
 r--r-----1204/70   3257 Nov 16 01:28 1983 src/cmd/sccs/src/sccshelp.c
 r--r-----1204/70  25902 Nov 16 01:28 1983 src/cmd/sccs/src/prs.c
 r--r-----1204/70   7269 Nov 16 01:28 1983 src/cmd/sccs/src/prt.c
 r--r-----1204/70  10909 Nov 16 01:28 1983 src/cmd/sccs/src/rmchg.c
 r--r-----1204/70  27843 Nov 16 01:28 1983 src/cmd/sccs/src/sccs.c
 r--r-----1204/70   4634 Nov 16 01:28 1983 src/cmd/sccs/src/unget.c
 r--r-----1204/70  15561 Nov 16 01:28 1983 src/cmd/sccs/src/val.c
 r--r-----1204/70  13116 Nov 16 01:28 1983 src/cmd/sccs/src/vc.c
 r--r-----1204/70   1381 Nov 16 01:28 1983 src/cmd/sccs/src/what.c
 r--r-----1204/70   1303 Nov 16 01:28 1983 src/cmd/sccs/src/sccsdiff


                                 433010137-001                     Page 38 of 93


                                  GENIX SOURCE


 rwxr-xr-x1204/70      0 Dec 15 16:50 1983 src/cmd/sccs/lib/
 rw-r-----1204/410    993 Dec  8 14:45 1983 src/cmd/sccs/lib/makefile
 r--r-----1204/410    707 Nov 10 09:53 1982 src/cmd/sccs/lib/auxf.c
 r--r-----1204/410    660 Nov 10 09:53 1982 src/cmd/sccs/lib/chkid.c
 r--r-----1204/410    373 Nov 10 09:53 1982 src/cmd/sccs/lib/chksid.c
 r--r-----1204/410   2142 Oct 17 13:22 1983 src/cmd/sccs/lib/date_ab.c
 r--r-----1204/410    558 Nov 10 09:53 1982 src/cmd/sccs/lib/date_ba.c
 r--r-----1204/410    774 Nov 10 09:53 1982 src/cmd/sccs/lib/del_ab.c
 r--r-----1204/410    669 Nov 10 09:53 1982 src/cmd/sccs/lib/del_ba.c
 r--r-----1204/410   3210 Nov 15 13:47 1982 src/cmd/sccs/lib/dodelt.c
 r--r-----1204/410   1240 Oct 17 13:22 1983 src/cmd/sccs/lib/dofile.c
 r--r-----1204/410    532 Nov 10 09:53 1982 src/cmd/sccs/lib/doie.c
 r--r-----1204/410   1766 Nov 10 09:53 1982 src/cmd/sccs/lib/dolist.c
 r--r-----1204/410    369 Nov 10 09:53 1982 src/cmd/sccs/lib/eqsid.c
 r--r-----1204/410    423 Nov 10 09:53 1982 src/cmd/sccs/lib/flushto.c
 r--r-----1204/410    322 Nov 10 09:53 1982 src/cmd/sccs/lib/fmterr.c
 r--r-----1204/410   1073 Nov 10 09:53 1982 src/cmd/sccs/lib/getline.c
 r--r-----1204/410    437 Nov 10 09:53 1982 src/cmd/sccs/lib/logname.c
 r--r-----1204/410    408 Nov 10 09:53 1982 src/cmd/sccs/lib/newstats.c
 r--r-----1204/410   3814 Nov 15 13:48 1982 src/cmd/sccs/lib/dohist.c
 r--r-----1204/410   1933 Nov 10 09:53 1982 src/cmd/sccs/lib/putline.c
 r--r-----1204/410   2317 Oct 17 13:34 1983 src/cmd/sccs/lib/permiss.c
 r--r-----1204/410   1625 Nov 10 09:54 1982 src/cmd/sccs/lib/setup.c
 r--r-----1204/410    440 Nov 10 09:54 1982 src/cmd/sccs/lib/sid_ab.c
 r--r-----1204/410    982 Nov 10 09:54 1982 src/cmd/sccs/lib/sinit.c
 r--r-----1204/410    405 Nov 10 09:54 1982 src/cmd/sccs/lib/sid_ba.c
 r--r-----1204/410    522 Nov 10 09:54 1982 src/cmd/sccs/lib/sidtoser.c
 r--r-----1204/410    506 Nov 10 09:54 1982 src/cmd/sccs/lib/stats_ab.c
 r--r-----1204/410   1213 Nov 15 13:50 1982 src/cmd/sccs/lib/pf_ab.c
 r--r-----1204/410   3383 Nov 15 13:51 1982 src/cmd/sccs/lib/rdmod.c
 rwxr-xr-x1204/70   4970 Nov  8 15:28 1983 src/cmd/sccs/makefile
 rwxr-xr-x1204/70      0 Dec 15 16:50 1983 src/cmd/sccs/help.d/
 rwxr-xr-x1204/70    481 Dec  8 20:06 1983 src/cmd/sccs/help.d/makefile
 r--r-----1204/70   4144 Nov 16 01:28 1983 src/cmd/sccs/help.d/ad
 r--r-----1204/70   1072 Nov 16 01:28 1983 src/cmd/sccs/help.d/bd
 r--r-----1204/70    648 Nov 16 01:28 1983 src/cmd/sccs/help.d/cb
 r--r-----1204/70   2171 Nov 16 01:28 1983 src/cmd/sccs/help.d/cm
 r--r-----1204/70   5431 Nov 16 01:29 1983 src/cmd/sccs/help.d/cmds
 r--r-----1204/70   3663 Nov 16 01:29 1983 src/cmd/sccs/help.d/co
 r--r-----1204/70   3333 Nov 16 01:29 1983 src/cmd/sccs/help.d/de
 r--r-----1204/70    150 Nov 16 01:29 1983 src/cmd/sccs/help.d/default
 r--r-----1204/70   2030 Nov 16 01:29 1983 src/cmd/sccs/help.d/ge
 r--r-----1204/70    309 Nov 16 01:29 1983 src/cmd/sccs/help.d/he
 r--r-----1204/70    490 Nov 16 01:29 1983 src/cmd/sccs/help.d/prs
 r--r-----1204/70   2261 Nov 16 01:29 1983 src/cmd/sccs/help.d/rc
 r--r-----1204/70    559 Nov 16 01:29 1983 src/cmd/sccs/help.d/un
 r--r-----1204/70   2070 Nov 16 01:29 1983 src/cmd/sccs/help.d/ut
 r--r-----1204/70   2417 Nov 16 01:29 1983 src/cmd/sccs/help.d/vc
 rwxr-xr-x1204/70   1440 Jun 10 20:44 1983 src/cmd/fold.c
 rwxr-xr-x1204/70   1922 Jun 10 20:44 1983 src/cmd/from.c
 rwxr-xr-x1204/70    583 Jun 10 20:45 1983 src/cmd/gets.c
 rwxr-xr-x1204/70   1396 Jun 10 20:45 1983 src/cmd/head.c
 rwxr-xr-x1204/70   3516 Jun 10 20:45 1983 src/cmd/last.c
 rwxr-xr-x1204/70   2605 Jun 10 20:46 1983 src/cmd/lastcomm.c
 rwxr-xr-x1204/70   3481 Jun 10 20:46 1983 src/cmd/leave.c
 rwxr-xr-x1204/70  12369 Jun 10 20:56 1983 src/cmd/w.c
 rwxr-xr-x1204/70   5114 Jun 10 20:47 1983 src/cmd/mkstr.c
 rwxr-xr-x1204/70  30541 Jun 10 20:48 1983 src/cmd/more.c


                                 433010137-001                     Page 39 of 93


                                  GENIX SOURCE


 rwxr-xr-x1204/70   1204 Jun 10 20:50 1983 src/cmd/num.c
 rwxr-xr-x1204/70    754 Jun 10 20:51 1983 src/cmd/printenv.c
 rwxr-xr-x1204/70   1157 Jun 10 20:51 1983 src/cmd/prmail.c
 rwxr-xr-x1204/70   1084 Nov 15 19:28 1983 src/cmd/help.sh
 rwxr-xr-x1204/70    367 Jun 10 20:52 1983 src/cmd/rewind.c
 rwxr-xr-x1204/70   1783 Jun 10 20:52 1983 src/cmd/soelim.c
 rwxr-xr-x1204/70   2067 Jun 10 20:52 1983 src/cmd/strings.c
 rwxr-xr-x1204/70   4814 Jun 10 20:53 1983 src/cmd/ul.c
 rwxr-xr-x1204/70   1455 Jun 10 20:53 1983 src/cmd/unexpand.c
 rwxr-xr-x1204/70   1086 Jun 10 20:54 1983 src/cmd/users.c
 rwxr-xr-x1204/70   3011 Jun 10 20:54 1983 src/cmd/uudecode.c
 rwxr-xr-x1204/70   1769 Jun 10 20:55 1983 src/cmd/uuencode.c
 rwxr-xr-x1204/70   3863 Jun 10 20:55 1983 src/cmd/uusend.c
 rwxr-xr-x1204/70      0 Dec 15 16:50 1983 src/cmd/lpr/
 r--r-----1204/70    876 Aug 17 15:45 1983 src/cmd/lpr/makefile
 r--r-----1204/70    191 Aug 17 15:45 1983 src/cmd/lpr/lpd.h
 r--r-----1204/70   4935 Aug 17 15:45 1983 src/cmd/lpr/lpd.c
 r--r-----1204/70   6159 Aug 31 00:59 1983 src/cmd/lpr/slpd.c
 r--r-----1204/70   5755 Aug 17 15:51 1983 src/cmd/lpr/lpf.c
 r--r-----1204/70   4902 Aug 17 15:57 1983 src/cmd/lpr/lpr.c
 rw-r--r--1204/70    425 Dec 10 01:49 1983 src/cmd/lpr/mklpr
 r--r-----1204/70   2279 Aug 17 16:00 1983 src/cmd/lpr/lpq.c
 r--r-----1204/70   2837 Aug 17 16:02 1983 src/cmd/lpr/lprm.c
 r--r-----1204/70   5726 Aug 17 16:10 1983 src/cmd/lpr/slpf.c
 rwxr-xr-x1204/70    127 Nov 15 19:34 1983 src/cmd/print.sh
 rwxr-xr-x1204/70    911 Jun 10 20:57 1983 src/cmd/what.c
 rwxr-xr-x1204/70   2254 Jun 10 20:58 1983 src/cmd/whatis.c
 rwxr-xr-x1204/70   4247 Jun 10 20:58 1983 src/cmd/whereis.c
 rwxr-xr-x1204/70      0 Dec 15 16:51 1983 src/cmd/diction/
 r--r-----1204/70   2042 Aug 17 11:10 1983 src/cmd/diction/makefile
 rw-r-----1204/70    613 Aug 12 15:21 1983 src/cmd/diction/style.sh
 r--r-----1204/70   9933 Jun 10 23:08 1983 src/cmd/diction/deroff.c
 r--r-----1204/70   8270 Jun 10 23:11 1983 src/cmd/diction/dict.d
 r--r-----1204/70    470 Jun 10 23:11 1983 src/cmd/diction/diction.sh
 r--r-----1204/70   6884 Jun 10 23:11 1983 src/cmd/diction/diction.c
 r--r-----1204/70  14219 Jun 10 23:12 1983 src/cmd/diction/explain.d
 r--r-----1204/70    327 Jun 10 23:12 1983 src/cmd/diction/explain.sh
 r--r-----1204/70   9572 Aug 12 18:44 1983 src/cmd/diction/ehash.c
 rw-r-----1204/70   6000 Aug 12 18:34 1983 src/cmd/diction/dict.c
 rw-r-----1204/70   2469 Aug 12 15:05 1983 src/cmd/diction/end.l
 rw-r-----1204/70    814 Aug 12 15:08 1983 src/cmd/diction/names.h
 rw-r-----1204/70   3751 Aug 12 18:48 1983 src/cmd/diction/nhash.c
 r--r-----1204/70   2153 Jun 10 23:08 1983 src/cmd/diction/README
 r--r-----1204/70   6207 Aug 12 18:36 1983 src/cmd/diction/nwords.l
 rw-r-----1204/70   5531 Aug 12 15:14 1983 src/cmd/diction/outp.c
 rw-r-----1204/70   6639 Aug 12 15:15 1983 src/cmd/diction/part.l
 r--r-----1204/70  17041 Aug 12 18:47 1983 src/cmd/diction/edict.c
 rw-r-----1204/70   9838 Aug 12 18:35 1983 src/cmd/diction/ydict.c
 rw-r-----1204/70    829 Aug 12 15:19 1983 src/cmd/diction/style.h
 r--r-----1204/70    349 Aug 12 17:58 1983 src/cmd/diction/conp.h
 r--r-----1204/70  15937 Aug 12 18:56 1983 src/cmd/diction/pscan.c
 rwxr-xr-x1204/70      0 Dec 15 17:27 1983 src/cmd/lint/
 rwxr-xr-x1204/70   1954 Dec  8 10:21 1983 src/cmd/lint/makefile
 r--r-----1204/70   6540 May 12 14:14 1983 src/cmd/lint/manifest
 r--r-----1204/70   5234 May 12 14:14 1983 src/cmd/lint/mfile1
 r--r-----1204/70   6672 May 12 14:14 1983 src/cmd/lint/common
 r--r-----1204/70   1724 May 12 14:14 1983 src/cmd/lint/macdefs
 r--r-----1204/70   1094 May 12 14:14 1983 src/cmd/lint/lmanifest


                                 433010137-001                     Page 40 of 93


                                  GENIX SOURCE


 r--r-----1204/70  21191 May 12 14:14 1983 src/cmd/lint/cgram.y
 r--r-----1204/70   2819 May 12 14:43 1983 src/cmd/lint/xdefs.c
 r--r-----1204/70  22325 May 12 14:44 1983 src/cmd/lint/scan.c
 r--r-----1204/70    191 May 12 14:55 1983 src/cmd/lint/comm1.c
 r--r-----1204/70  40917 May 12 14:57 1983 src/cmd/lint/pftn.c
 r--r-----1204/70  31729 May 12 15:15 1983 src/cmd/lint/trees.c
 r--r-----1204/70   4062 May 12 15:42 1983 src/cmd/lint/optim.c
 r--r-----1204/70  20860 May 12 15:46 1983 src/cmd/lint/lint.c
 r--r-----1204/70  13083 May 12 16:41 1983 src/cmd/lint/lpass2.c
 r--r-----1204/70   1710 May 12 16:48 1983 src/cmd/lint/llib-port
 r--r-----1204/70   3983 May 12 16:48 1983 src/cmd/lint/llib-lc
 r--r-----1204/70    627 Nov 16 01:05 1983 src/cmd/lint/SHELL
 rwxr-xr-x1204/70      0 Dec 15 16:51 1983 src/cmd/grep/
 r--r-----1204/70    725 Aug 15 18:00 1983 src/cmd/grep/makefile
 r--r-----1204/70   7034 Jun 11 00:03 1983 src/cmd/grep/btlgrep.c
 r--r-----1204/70   6099 Jun 11 00:04 1983 src/cmd/grep/ucbgrep.c
 rwxr-xr-x1204/70      0 Dec 15 16:51 1983 src/cmd/m4/
 r--r-----1204/70    718 Sep 16 16:59 1983 src/cmd/m4/makefile
 r--r-----1204/70  15499 Jun 11 00:16 1983 src/cmd/m4/m4.c
 r--r-----1204/70   1787 Jun 11 00:17 1983 src/cmd/m4/m4y.y
 rwxr-xr-x1204/70      0 Dec 15 16:51 1983 src/cmd/eqn/
 r--r-----1204/70   1079 Aug 17 11:12 1983 src/cmd/eqn/makefile
 r--r-----1204/70   1187 Jun 10 23:23 1983 src/cmd/eqn/e.h
 r--r-----1204/70   4147 Jun 10 23:23 1983 src/cmd/eqn/e.y
 r--r-----1204/70   1867 Jun 10 23:24 1983 src/cmd/eqn/diacrit.c
 r--r-----1204/70    729 Jun 10 23:24 1983 src/cmd/eqn/eqnbox.c
 r--r-----1204/70   1354 Jun 10 23:25 1983 src/cmd/eqn/font.c
 r--r-----1204/70   1498 Jun 10 23:25 1983 src/cmd/eqn/fromto.c
 r--r-----1204/70    760 Jun 10 23:25 1983 src/cmd/eqn/funny.c
 r--r-----1204/70   1057 Jun 10 23:25 1983 src/cmd/eqn/glob.c
 r--r-----1204/70    855 Jun 10 23:25 1983 src/cmd/eqn/integral.c
 r--r-----1204/70   4336 Jun 10 23:25 1983 src/cmd/eqn/io.c
 r--r-----1204/70   4412 Jun 10 23:26 1983 src/cmd/eqn/lex.c
 r--r-----1204/70   4410 Jun 10 23:26 1983 src/cmd/eqn/lookup.c
 r--r-----1204/70    452 Jun 10 23:27 1983 src/cmd/eqn/mark.c
 r--r-----1204/70   1476 Jun 10 23:27 1983 src/cmd/eqn/matrix.c
 r--r-----1204/70    669 Jun 10 23:27 1983 src/cmd/eqn/move.c
 r--r-----1204/70   1061 Jun 10 23:27 1983 src/cmd/eqn/over.c
 r--r-----1204/70   2495 Jun 10 23:28 1983 src/cmd/eqn/paren.c
 r--r-----1204/70   1814 Jun 10 23:28 1983 src/cmd/eqn/pile.c
 r--r-----1204/70   3192 Jun 10 23:28 1983 src/cmd/eqn/shift.c
 r--r-----1204/70   1028 Jun 10 23:29 1983 src/cmd/eqn/size.c
 r--r-----1204/70    658 Jun 10 23:29 1983 src/cmd/eqn/sqrt.c
 r--r-----1204/70   3444 Jun 10 23:29 1983 src/cmd/eqn/text.c
 rwxr-xr-x1204/70      0 Dec 15 16:52 1983 src/cmd/neqn/
 r--r-----1204/70   1086 Aug 17 11:19 1983 src/cmd/neqn/makefile
 r--r-----1204/70   1182 Jun 11 00:24 1983 src/cmd/neqn/e.h
 r--r-----1204/70   4146 Jun 11 00:24 1983 src/cmd/neqn/e.y
 r--r-----1204/70   1164 Jun 11 00:25 1983 src/cmd/neqn/diacrit.c
 r--r-----1204/70    729 Jun 11 00:25 1983 src/cmd/neqn/eqnbox.c
 r--r-----1204/70   1346 Jun 11 00:25 1983 src/cmd/neqn/font.c
 r--r-----1204/70   1501 Jun 11 00:26 1983 src/cmd/neqn/fromto.c
 r--r-----1204/70    675 Jun 11 00:26 1983 src/cmd/neqn/funny.c
 r--r-----1204/70   1057 Jun 11 00:26 1983 src/cmd/neqn/glob.c
 r--r-----1204/70    790 Jun 11 00:26 1983 src/cmd/neqn/integral.c
 r--r-----1204/70   4335 Jun 11 00:26 1983 src/cmd/neqn/io.c
 r--r-----1204/70   4412 Jun 11 00:27 1983 src/cmd/neqn/lex.c
 r--r-----1204/70   4237 Jun 11 00:27 1983 src/cmd/neqn/lookup.c


                                 433010137-001                     Page 41 of 93


                                  GENIX SOURCE


 r--r-----1204/70    452 Jun 11 00:28 1983 src/cmd/neqn/mark.c
 r--r-----1204/70   1476 Jun 11 00:28 1983 src/cmd/neqn/matrix.c
 r--r-----1204/70    689 Jun 11 00:28 1983 src/cmd/neqn/move.c
 r--r-----1204/70    951 Jun 11 00:28 1983 src/cmd/neqn/over.c
 r--r-----1204/70   2485 Jun 11 00:29 1983 src/cmd/neqn/paren.c
 r--r-----1204/70   1762 Jun 11 00:29 1983 src/cmd/neqn/pile.c
 r--r-----1204/70   2108 Jun 11 00:29 1983 src/cmd/neqn/shift.c
 r--r-----1204/70   1028 Jun 11 00:30 1983 src/cmd/neqn/size.c
 r--r-----1204/70    484 Jun 11 00:30 1983 src/cmd/neqn/sqrt.c
 r--r-----1204/70   3422 Jun 11 00:30 1983 src/cmd/neqn/text.c
 rwxr-xr-x1204/70      0 Dec 15 16:52 1983 src/cmd/yacc/
 r--r-----1204/70    922 Aug 25 17:38 1983 src/cmd/yacc/makefile
 r--r-----1204/70   5872 Aug 25 17:38 1983 src/cmd/yacc/dextern
 r--r-----1204/70    638 Aug 25 17:38 1983 src/cmd/yacc/files
 r--r-----1204/70  16727 Nov 10 16:16 1983 src/cmd/yacc/y1.c
 r--r-----1204/70  19062 Aug 25 17:38 1983 src/cmd/yacc/y2.c
 r--r-----1204/70   9377 Aug 25 17:38 1983 src/cmd/yacc/y3.c
 r--r-----1204/70   6372 Aug 25 17:38 1983 src/cmd/yacc/y4.c
 r--r-----1204/70   3488 Aug 25 17:38 1983 src/cmd/yacc/yaccpar
 rw-r-----1204/70  48833 Aug 10 19:24 1983 src/cmd/yacc/y1.s
 r--r-----1204/70   4883 Aug 25 17:38 1983 src/cmd/yacc/yaccdiffs
 r--r-----1204/70   6868 Aug 25 17:38 1983 src/cmd/yacc/yaccnews
 rwxr-xr-x1204/70      0 Dec 15 16:52 1983 src/cmd/tbl/
 r--r-----1204/70    819 Aug 25 11:29 1983 src/cmd/tbl/makefile
 r--r-----1204/70   1699 Jun 14 01:26 1983 src/cmd/tbl/t..c
 r--r-----1204/70   1224 Jun 14 01:26 1983 src/cmd/tbl/t0.c
 r--r-----1204/70   1977 Jun 14 01:26 1983 src/cmd/tbl/t1.c
 r--r-----1204/70    409 Jun 14 01:26 1983 src/cmd/tbl/t2.c
 r--r-----1204/70   1850 Jun 14 01:26 1983 src/cmd/tbl/t3.c
 r--r-----1204/70   5407 Jun 14 01:27 1983 src/cmd/tbl/t4.c
 r--r-----1204/70   3277 Jun 14 01:27 1983 src/cmd/tbl/t5.c
 r--r-----1204/70   6358 Jun 14 01:28 1983 src/cmd/tbl/t6.c
 r--r-----1204/70   3268 Jun 14 01:28 1983 src/cmd/tbl/t7.c
 r--r-----1204/70   7455 Jun 14 01:29 1983 src/cmd/tbl/t8.c
 r--r-----1204/70   1505 Jun 14 01:30 1983 src/cmd/tbl/t9.c
 r--r-----1204/70   1633 Jun 14 01:30 1983 src/cmd/tbl/tb.c
 r--r-----1204/70   1260 Jun 14 01:30 1983 src/cmd/tbl/tc.c
 r--r-----1204/70   1187 Jun 14 01:30 1983 src/cmd/tbl/te.c
 r--r-----1204/70   1284 Jun 14 01:31 1983 src/cmd/tbl/tf.c
 r--r-----1204/70   2026 Jun 14 01:31 1983 src/cmd/tbl/tg.c
 r--r-----1204/70   1165 Jun 14 01:31 1983 src/cmd/tbl/ti.c
 r--r-----1204/70   1201 Jun 14 01:31 1983 src/cmd/tbl/tm.c
 r--r-----1204/70    823 Jun 14 01:32 1983 src/cmd/tbl/ts.c
 r--r-----1204/70   1547 Jun 14 01:32 1983 src/cmd/tbl/tt.c
 r--r-----1204/70   4198 Jun 14 01:32 1983 src/cmd/tbl/tu.c
 r--r-----1204/70   3012 Jun 14 01:33 1983 src/cmd/tbl/tv.c
 rwxr-xr-x1204/70      0 Dec 15 16:52 1983 src/cmd/lex/
 r--r-----1204/70    935 Aug 16 10:22 1983 src/cmd/lex/makefile
 r--r-----1204/70   3403 Jun 11 00:04 1983 src/cmd/lex/ldefs.c
 r--r-----1204/70   3551 Jun 11 00:04 1983 src/cmd/lex/once.c
 r--r-----1204/70   5885 Jun 11 00:04 1983 src/cmd/lex/lmain.c
 r--r-----1204/70  15609 Jun 11 00:05 1983 src/cmd/lex/parser.y
 r--r-----1204/70  12361 Jun 11 00:07 1983 src/cmd/lex/sub1.c
 r--r-----1204/70  20584 Jun 11 00:08 1983 src/cmd/lex/sub2.c
 r--r-----1204/70   3909 Jun 11 00:10 1983 src/cmd/lex/header.c
 r--r-----1204/70   4009 Jul 26 17:52 1983 src/cmd/lex/ncform
 rwxr-xr-x1204/70    384 Jun 10 20:59 1983 src/cmd/whoami.c
 rwxr-xr-x1204/70    232 Jun 10 20:59 1983 src/cmd/yes.c


                                 433010137-001                     Page 42 of 93


                                  GENIX SOURCE


 rwxr-xr-x1204/70   2126 Jun 10 20:59 1983 src/cmd/atrun.c
 rwxr-xr-x1204/70   1755 Jun 10 21:00 1983 src/cmd/getNAME.c
 rwxr-xr-x1204/70    131 Nov 15 19:36 1983 src/cmd/see.sh
 rwxr-xr-x1204/70      0 Dec 15 16:52 1983 src/cmd/pstat/
 r--r-----1204/70   1154 Sep 16 16:56 1983 src/cmd/pstat/makefile
 r--r-----1204/70  15981 Oct  3 14:02 1983 src/cmd/pstat/pstat.c
 r--r-----1204/70    528 Aug 17 17:02 1983 src/cmd/pstat/bitcount.s
 rwxr-xr-x1204/70    442 Jun 10 21:00 1983 src/cmd/makekey.c
 rwxr-xr-x1204/70      0 Dec 15 16:53 1983 src/cmd/as/
 r--r-----1204/70   8144 Sep  9 19:27 1983 src/cmd/as/makefile
 r--r-----1204/70   1893 Sep 26 23:27 1983 src/cmd/as/e2bio.e
 r--r-----1204/70  38936 Sep 26 23:27 1983 src/cmd/as/asops.s
 rw-r-----1204/70   1458 Nov 10 17:13 1983 src/cmd/as/result
 r--r-----1204/70   2005 Sep 26 23:28 1983 src/cmd/as/glob_const.e
 r--r-----1204/70   4932 Sep 26 23:28 1983 src/cmd/as/formatsmod.e
 r--r-----1204/70  10737 Sep 26 23:28 1983 src/cmd/as/glob_types.e
 r--r-----1204/70   1840 Sep 26 23:28 1983 src/cmd/as/glob_vars.e
 r--r-----1204/70    283 Sep 26 23:28 1983 src/cmd/as/mnem_def.e
 r--r-----1204/70    751 Sep 26 23:28 1983 src/cmd/as/scantoken.e
 r--r-----1204/70    829 Sep 26 23:28 1983 src/cmd/as/scanexp.e
 r--r-----1204/70   1303 Sep 26 23:28 1983 src/cmd/as/scanr.e
 r--r-----1204/70   1537 Sep 26 23:28 1983 src/cmd/as/dirs.e
 r--r-----1204/70    449 Sep 26 23:28 1983 src/cmd/as/insts.e
 r--r-----1204/70    575 Sep 26 23:28 1983 src/cmd/as/scan.e
 r--r-----1204/70    865 Sep 26 23:28 1983 src/cmd/as/dirsx.e
 r--r-----1204/70    857 Sep 26 23:28 1983 src/cmd/as/prsymtable.e
 r--r-----1204/70   1523 Sep 26 23:28 1983 src/cmd/as/sdis.e
 r--r-----1204/70    444 Sep 26 23:28 1983 src/cmd/as/codegen.e
 r--r-----1204/70    621 Sep 26 23:28 1983 src/cmd/as/genrout.e
 r--r-----1204/70    211 Sep 26 23:28 1983 src/cmd/as/traverse.e
 r--r-----1204/70   1603 Sep 26 23:28 1983 src/cmd/as/disp.e
 r--r-----1204/70   1080 Sep 26 23:28 1983 src/cmd/as/init1.e
 r--r-----1204/70   1973 Sep 26 23:28 1983 src/cmd/as/a.out.e
 r--r-----1204/70   6785 Sep 26 23:28 1983 src/cmd/as/stab.e
 r--r-----1204/70  20441 Sep 26 23:28 1983 src/cmd/as/init1.p
 rw-r-----1204/70   1893 Dec  9 20:48 1983 src/cmd/as/bio.e
 r--r-----1204/70    501 Sep 26 23:29 1983 src/cmd/as/list_types.e
 r--r-----1204/70   6272 Sep 26 23:29 1983 src/cmd/as/scan.p
 r--r-----1204/70    147 Sep 26 23:30 1983 src/cmd/as/scmpc.e
 r--r-----1204/70   7931 Sep 26 23:30 1983 src/cmd/as/prsymtable.p
 r--r-----1204/70  32092 Sep 26 23:30 1983 src/cmd/as/scanr.p
 r--r-----1204/70  42872 Sep 26 23:32 1983 src/cmd/as/scanexp.p
 r--r-----1204/70    332 Sep 26 23:33 1983 src/cmd/as/cmpc.e
 r--r-----1204/70    329 Sep 26 23:33 1983 src/cmd/as/float.e
 r--r-----1204/70  29458 Sep 26 23:33 1983 src/cmd/as/insts.p
 r--r-----1204/70  58616 Sep 26 23:34 1983 src/cmd/as/dirs.p
 r--r-----1204/70  15244 Sep 26 23:36 1983 src/cmd/as/sdis.p
 r--r-----1204/70   5584 Sep 26 23:36 1983 src/cmd/as/genrout.p
 r--r-----1204/70  65681 Sep 26 23:37 1983 src/cmd/as/codegen.p
 r--r-----1204/70  10559 Sep 26 23:40 1983 src/cmd/as/dirsx.p
 r--r-----1204/70   3274 Sep 26 23:41 1983 src/cmd/as/traverse.p
 r--r-----1204/70    271 Sep 26 23:41 1983 src/cmd/as/cmpc.s
 r--r-----1204/70    701 Sep 26 23:41 1983 src/cmd/as/scmpc.s
 r--r-----1204/70  18278 Sep 26 23:41 1983 src/cmd/as/disp.p
 r--r-----1204/70  13157 Sep 26 23:43 1983 src/cmd/as/float.p
 r--r-----1204/70    174 Sep 26 23:43 1983 src/cmd/as/dumbio.p
 r--r-----1204/70    405 Sep 26 23:43 1983 src/cmd/as/fopenfile2.p
 r--r-----1204/70  18711 Sep 26 23:43 1983 src/cmd/as/scantoken.p


                                 433010137-001                     Page 43 of 93


                                  GENIX SOURCE


 r--r-----1204/70    239 Sep 26 23:44 1983 src/cmd/as/pexit.s
 r--r-----1204/70  10152 Sep 26 23:44 1983 src/cmd/as/asm.p
 r--r-----1204/70    255 Sep 26 23:45 1983 src/cmd/as/ptime.s
 r--r-----1204/70   4125 Sep 26 23:45 1983 src/cmd/as/genbytes.p
 r--r-----1204/70    203 Sep 26 23:45 1983 src/cmd/as/fnew.s
 r--r-----1204/70  23326 Sep 26 23:46 1983 src/cmd/as/lst.p
 rw-r-----1204/70  44510 Dec  9 20:48 1983 src/cmd/as/ldmap
 rw-r--r--1204/70 137982 Dec  9 20:48 1983 src/cmd/as/asm2
 r--r-----1204/70   3277 Sep 26 23:47 1983 src/cmd/as/n_asm.c
 r--r-----1204/70    414 Sep 26 23:47 1983 src/cmd/as/as.h
 rw-r-----1204/70    414 Sep 26 23:47 1983 src/cmd/as/n_asm.h
 rwxr-xr-x1204/70    657 Jun 10 21:00 1983 src/cmd/burn.c
 rwxr-xr-x1204/70   3261 Jun 10 21:01 1983 src/cmd/csym.c
 rwxr-xr-x1204/70  11953 Jun 10 21:01 1983 src/cmd/cu16.c
 rwxr-xr-x1204/70    222 Nov 15 19:37 1983 src/cmd/trman.sh
 rwxr-xr-x1204/70   3937 Jun 10 21:02 1983 src/cmd/dcerr.c
 rwxr-xr-x1204/70   2488 Aug 17 09:59 1983 src/cmd/monfix.c
 rwxr-xr-x1204/70      0 Dec 15 16:54 1983 src/cmd/uucp/
 rwxr-xr-x1204/70  10564 Dec  8 08:57 1983 src/cmd/uucp/makefile
 r--r-----1204/70   3293 Jun 11 01:33 1983 src/cmd/uucp/anlwrk.c
 r--r-----1204/70    483 Jun 11 01:33 1983 src/cmd/uucp/anyread.c
 r--r-----1204/70   4401 Jun 11 01:33 1983 src/cmd/uucp/chkpth.c
 r--r-----1204/70    571 Jun 11 01:33 1983 src/cmd/uucp/chksum.c
 r--r-----1204/70   8853 Jun 11 01:33 1983 src/cmd/uucp/cico.c
 r--r-----1204/70  16592 Jul  1 14:21 1983 src/cmd/uucp/cntrl.c
 r--r-----1204/70  16568 Aug 14 16:24 1983 src/cmd/uucp/conn.c
 r--r-----1204/70   1165 Jun 11 01:34 1983 src/cmd/uucp/cpmv.c
 r--r-----1204/70   1600 Jun 11 01:34 1983 src/cmd/uucp/expfile.c
 r--r-----1204/70   1334 Jun 11 01:34 1983 src/cmd/uucp/gename.c
 r--r-----1204/70    720 Jun 11 01:34 1983 src/cmd/uucp/getargs.c
 r--r-----1204/70   1189 Jun 11 01:34 1983 src/cmd/uucp/getopt.c
 r--r-----1204/70   1840 Jun 11 01:34 1983 src/cmd/uucp/getprm.c
 r--r-----1204/70    704 Jun 11 01:34 1983 src/cmd/uucp/getpw.c
 r--r-----1204/70   1025 Jun 11 01:34 1983 src/cmd/uucp/getpwinfo.c
 r--r-----1204/70   4723 Jun 11 01:34 1983 src/cmd/uucp/gio.c
 r--r-----1204/70    734 Jun 11 01:34 1983 src/cmd/uucp/gnamef.c
 r--r-----1204/70   2329 Jun 11 01:34 1983 src/cmd/uucp/gnsys.c
 r--r-----1204/70   1830 Jun 11 01:34 1983 src/cmd/uucp/gnxseq.c
 r--r-----1204/70    521 Jun 11 01:34 1983 src/cmd/uucp/gwd.c
 r--r-----1204/70   1266 Jun 11 01:34 1983 src/cmd/uucp/imsg.c
 r--r-----1204/70    453 Jun 11 01:34 1983 src/cmd/uucp/index.c
 r--r-----1204/70    625 Jun 11 01:34 1983 src/cmd/uucp/ioctl.c
 r--r-----1204/70    398 Jun 11 01:34 1983 src/cmd/uucp/lastpart.c
 r--r-----1204/70   2085 Jun 11 01:34 1983 src/cmd/uucp/logent.c
 r--r-----1204/70    839 Jun 11 01:34 1983 src/cmd/uucp/mailst.c
 r--r-----1204/70    925 Jun 11 01:34 1983 src/cmd/uucp/pfopen.c
 r--r-----1204/70  10516 Jun 11 01:34 1983 src/cmd/uucp/pk0.c
 r--r-----1204/70   7623 Jun 11 01:34 1983 src/cmd/uucp/pk1.c
 r--r-----1204/70    153 Jun 11 01:34 1983 src/cmd/uucp/pkon.c
 r--r-----1204/70    370 Jun 11 01:34 1983 src/cmd/uucp/prefix.c
 r--r-----1204/70   1168 Jun 11 01:34 1983 src/cmd/uucp/sdmail.c
 r--r-----1204/70   1119 Jun 11 01:34 1983 src/cmd/uucp/shio.c
 r--r-----1204/70    312 Jun 11 01:34 1983 src/cmd/uucp/sysacct.c
 r--r-----1204/70   2309 Jun 11 01:34 1983 src/cmd/uucp/systat.c
 r--r-----1204/70   1432 Jun 11 01:34 1983 src/cmd/uucp/ub_sst.c
 r--r-----1204/70   3954 Jun 11 01:34 1983 src/cmd/uucp/ulockf.c
 r--r-----1204/70   1656 Jun 11 01:34 1983 src/cmd/uucp/us_crs.c
 r--r-----1204/70   1504 Jun 11 01:34 1983 src/cmd/uucp/us_open.c


                                 433010137-001                     Page 44 of 93


                                  GENIX SOURCE


 r--r-----1204/70   1736 Jun 11 01:34 1983 src/cmd/uucp/us_rrs.c
 r--r-----1204/70   1430 Jun 11 01:34 1983 src/cmd/uucp/us_sst.c
 r--r-----1204/70   3929 Jun 11 01:34 1983 src/cmd/uucp/uuclean.c
 r--r-----1204/70   8180 Jun 11 01:34 1983 src/cmd/uucp/uucp.c
 r--r-----1204/70    445 Jun 11 01:34 1983 src/cmd/uucp/uucpdefs.c
 r--r-----1204/70    704 Jun 11 01:34 1983 src/cmd/uucp/uucpname.c
 r--r-----1204/70   3213 Jun 11 01:34 1983 src/cmd/uucp/uulog.c
 r--r-----1204/70   1332 Jun 11 01:34 1983 src/cmd/uucp/uuname.c
 r--r-----1204/70   1450 Jun 11 01:34 1983 src/cmd/uucp/uurecover.c
 r--r-----1204/70   1745 Jun 11 01:34 1983 src/cmd/uucp/uusecure.c
 r--r-----1204/70   7338 Jun 11 01:35 1983 src/cmd/uucp/uustat.c
 r--r-----1204/70   5994 Jun 11 01:35 1983 src/cmd/uucp/uusub.c
 r--r-----1204/70   7844 Jun 11 01:35 1983 src/cmd/uucp/uux.c
 r--r-----1204/70   9018 Jun 11 01:35 1983 src/cmd/uucp/uuxqt.c
 r--r-----1204/70    763 Jun 11 01:35 1983 src/cmd/uucp/versys.c
 r--r-----1204/70   1576 Jun 11 01:35 1983 src/cmd/uucp/xqt.c
 r--r-----1204/70   2333 Jun 11 01:35 1983 src/cmd/uucp/pk.h
 r--r-----1204/70   3686 Jun 11 01:35 1983 src/cmd/uucp/uucp.h
 r--r-----1204/70    334 Jun 11 01:35 1983 src/cmd/uucp/uucpdefs.h
 r--r-----1204/70   2329 Jun 11 01:35 1983 src/cmd/uucp/uust.h
 r--r-----1204/70   1305 Jun 11 01:35 1983 src/cmd/uucp/uusub.h
 r--r-----1204/70   1911 Jun 11 01:35 1983 src/cmd/uucp/pk.p
 rwxr-xr-x1204/70  31031 Jun 10 21:04 1983 src/cmd/remind.p
 rwxr-xr-x1204/70      0 Dec 15 16:54 1983 src/cmd/delivermail/
 rw-r-----1204/70   7929 Feb 14 15:12 1983 src/cmd/delivermail/READ_ME
 rw-r-----1204/70   1308 Feb 14 15:12 1983 src/cmd/delivermail/TO_BE_DONE
 r--r-----1204/70   3247 Nov 17 11:48 1983 src/cmd/delivermail/makefile
 r--r-----1204/70    309 Oct 31 12:10 1983 src/cmd/delivermail/useful.h
 r--r-----1204/70   4491 Oct 31 12:10 1983 src/cmd/delivermail/dlvrmail.h
 r--r-----1204/70  12787 Oct 31 12:10 1983 src/cmd/delivermail/conf.c
 r--r-----1204/70  11707 Oct 31 12:11 1983 src/cmd/delivermail/main.c
 r--r-----1204/70   5387 Oct 31 12:11 1983 src/cmd/delivermail/maketemp.c
 r--r-----1204/70   1597 Oct 31 15:45 1983 src/cmd/delivermail/matchhdr.c
 r--r-----1204/70  10537 Oct 31 12:12 1983 src/cmd/delivermail/parse.c
 r--r-----1204/70   6289 Oct 31 12:12 1983 src/cmd/delivermail/alias.c
 r--r-----1204/70  15790 Oct 31 12:13 1983 src/cmd/delivermail/deliver.c
 r--r-----1204/70   5720 Oct 31 12:14 1983 src/cmd/delivermail/savemail.c
 r--r-----1204/70   2405 Oct 31 12:14 1983 src/cmd/delivermail/addr.c
 rw-r-----1204/70   1685 Oct 31 15:40 1983 src/cmd/delivermail/err.c
 r--r-----1204/70    717 Oct 31 15:46 1983 src/cmd/delivermail/sysexits.c
 r--r-----1204/70   1756 Oct 31 15:46 1983 src/cmd/delivermail/util.c
 r--r-----1204/70    281 Oct 31 15:46 1983 src/cmd/delivermail/bmove.c
 r--r-----1204/70    221 Oct 31 15:46 1983 src/cmd/delivermail/version.c
 r--r-----1204/70   3632 Oct 31 19:35 1983 src/cmd/delivermail/newaliases.c
 r--r-----1204/70     20 Oct 31 15:46 1983 src/cmd/delivermail/Mail.rc
 r--r-----1204/70  11056 Nov 17 11:49 1983 src/cmd/delivermail/arpa.c
 r--r-----1204/70   1044 Nov 17 11:50 1983 src/cmd/delivermail/arpadate.c
 rwxr-xr-x1204/70    778 Nov 15 19:44 1983 src/cmd/which.sh
 rwxr-xr-x1204/70   1481 Jun 10 21:05 1983 src/cmd/su1.c
 rwxr-xr-x1204/70      0 Dec 15 16:54 1983 src/cmd/ld/
 r--r-----1204/70   1470 Sep 10 09:37 1983 src/cmd/ld/symbols.h
 r--r-----1204/70   1122 Aug 25 16:44 1983 src/cmd/ld/ross.h
 r--r-----1204/70   5502 Aug 25 16:44 1983 src/cmd/ld/meld2.h
 r--r-----1204/70   1443 Aug 25 16:43 1983 src/cmd/ld/lookup.h
 r--r-----1204/70   2659 Sep 16 13:54 1983 src/cmd/ld/makefile
 r--r-----1204/70   1410 Aug 25 16:44 1983 src/cmd/ld/system.h
 rw-r-----1204/70  38891 Oct 31 10:14 1983 src/cmd/ld/meld.c
 r--r-----1204/70   5716 Aug 25 16:44 1983 src/cmd/ld/symbols.c


                                 433010137-001                     Page 45 of 93


                                  GENIX SOURCE


 rwxr-x---1204/70  11537 Sep 20 13:41 1983 src/cmd/ld/libs.c
 r--r-----1204/70   4366 Aug 25 16:43 1983 src/cmd/ld/lookup.c
 r--r-----1204/70   1768 Aug 25 16:42 1983 src/cmd/ld/alloc.c
 r--r-----1204/70   3283 Aug 25 16:42 1983 src/cmd/ld/flsbuf.c
 r--r-----1204/70   2492 Aug 25 16:44 1983 src/cmd/ld/rdwr.as
 r--r-----1204/70   2488 Aug 25 16:44 1983 src/cmd/ld/rdwr.s
 r--r-----1204/70   1913 Aug 26 09:13 1983 src/cmd/ld/mrw.s
 rwxr-xr-x1204/70      0 Dec 15 16:55 1983 src/cmd/c2/
 r--r-----1204/70   2700 Nov  9 17:53 1983 src/cmd/c2/makefile
 r--r-----1204/70   8308 Nov  9 17:53 1983 src/cmd/c2/c2.h
 r--r-----1204/70  35799 Nov  9 17:53 1983 src/cmd/c2/c20.c
 r--r-----1204/70  86635 Nov  9 17:54 1983 src/cmd/c2/c21.c
 r--r-----1204/70   8731 Nov  9 17:54 1983 src/cmd/c2/c22.c
 r--r-----1204/70    340 Nov  9 17:54 1983 src/cmd/c2/c2vers.c
 r-xr-x---1204/70    274 Nov  9 17:52 1983 src/cmd/c2/rofix
 rwxr-xr-x1204/70      0 Dec 15 16:55 1983 src/cmd/ddt/
 r--r-----1204/70   5709 Oct 31 19:21 1983 src/cmd/ddt/makefile
 r--r-----1204/70   1701 Oct 31 19:24 1983 src/cmd/ddt/main.h
 r--r-----1204/70    837 Oct 31 19:24 1983 src/cmd/ddt/error.h
 r--r-----1204/70    692 Oct 31 19:24 1983 src/cmd/ddt/scan.h
 r--r-----1204/70   1420 Oct 31 19:24 1983 src/cmd/ddt/parse.h
 r--r-----1204/70    182 Oct 31 19:24 1983 src/cmd/ddt/symbol.h
 r--r-----1204/70    538 Oct 31 19:24 1983 src/cmd/ddt/display.h
 r--r-----1204/70    230 Oct 31 19:24 1983 src/cmd/ddt/bpt.h
 r--r-----1204/70    478 Oct 31 19:24 1983 src/cmd/ddt/machine.h
 r--r-----1204/70  25858 Oct 31 19:24 1983 src/cmd/ddt/main.c
 r--r-----1204/70  14746 Oct 31 19:26 1983 src/cmd/ddt/display.c
 r--r-----1204/70   8226 Oct 31 19:27 1983 src/cmd/ddt/symbol.c
 r--r-----1204/70   5396 Oct 31 19:28 1983 src/cmd/ddt/scan.c
 r--r-----1204/70   8557 Oct 31 19:28 1983 src/cmd/ddt/parse.c
 r--r-----1204/70   8809 Oct 31 19:29 1983 src/cmd/ddt/mem.c
 r--r-----1204/70   8138 Oct 31 19:30 1983 src/cmd/ddt/reg.c
 r--r-----1204/70   7748 Oct 31 19:31 1983 src/cmd/ddt/process.c
 r--r-----1204/70   9032 Oct 31 19:32 1983 src/cmd/ddt/bpt.c
 r--r-----1204/70   3217 Oct 31 19:33 1983 src/cmd/ddt/stops.c
 r--r-----1204/70   1309 Oct 31 19:34 1983 src/cmd/ddt/call.c
 r--r-----1204/70  15108 Oct 31 19:34 1983 src/cmd/ddt/machine.c
 r--r-----1204/70   3753 Oct 31 19:35 1983 src/cmd/ddt/serial.c
 r--r-----1204/70   2371 Oct 31 19:36 1983 src/cmd/ddt/gpib.c
 rwxr-xr-x1204/70   2319 Jul 12 14:20 1983 src/cmd/mmerr.c
 rwxr-xr-x1204/70      0 Dec 15 16:56 1983 src/cmd/ucbmail/
 rw-r-----1204/70      0 Dec 11 00:08 1983 src/cmd/ucbmail/strings
 r--r-----1204/70   2611 Aug 25 11:31 1983 src/cmd/ucbmail/makefile
 r--r-----1204/70  10329 Jun 11 01:19 1983 src/cmd/ucbmail/aux.c
 r--r-----1204/70   4991 Jun 11 01:19 1983 src/cmd/ucbmail/cmd1.c
 r--r-----1204/70   6708 Jun 11 01:19 1983 src/cmd/ucbmail/cmd2.c
 r--r-----1204/70   8588 Jun 11 01:19 1983 src/cmd/ucbmail/cmd3.c
 r--r-----1204/70   2435 Jun 11 01:19 1983 src/cmd/ucbmail/cmdtab.c
 r--r-----1204/70  14426 Jun 11 01:19 1983 src/cmd/ucbmail/collect.c
 r--r-----1204/70   3437 Jun 11 01:19 1983 src/cmd/ucbmail/edit.c
 r--r-----1204/70   7401 Jun 11 01:19 1983 src/cmd/ucbmail/fio.c
 r--r-----1204/70   7170 Jun 11 01:19 1983 src/cmd/ucbmail/fmt.c
 r--r-----1204/70   3271 Jun 11 01:19 1983 src/cmd/ucbmail/getname.c
 r--r-----1204/70   5383 Jun 11 01:19 1983 src/cmd/ucbmail/head.c
 r--r-----1204/70    985 Jun 11 01:19 1983 src/cmd/ucbmail/v7.local.c
 r--r-----1204/70   1963 Jun 11 01:19 1983 src/cmd/ucbmail/lock.c
 r--r-----1204/70  16953 Jun 11 01:19 1983 src/cmd/ucbmail/optim.c
 r--r-----1204/70  14385 Jun 11 01:19 1983 src/cmd/ucbmail/names.c


                                 433010137-001                     Page 46 of 93


                                  GENIX SOURCE


 r--r-----1204/70   7427 Jun 11 01:19 1983 src/cmd/ucbmail/lex.c
 r--r-----1204/70   9170 Jun 11 01:19 1983 src/cmd/ucbmail/list.c
 r--r-----1204/70   5331 Jun 11 01:19 1983 src/cmd/ucbmail/main.c
 r--r-----1204/70   5929 Jun 11 01:19 1983 src/cmd/ucbmail/quit.c
 r--r-----1204/70   7917 Jun 11 01:19 1983 src/cmd/ucbmail/send.c
 r--r-----1204/70   1894 Jun 11 01:19 1983 src/cmd/ucbmail/strings.c
 r--r-----1204/70   1499 Jun 11 01:19 1983 src/cmd/ucbmail/temp.c
 r--r-----1204/70   3386 Jun 11 01:19 1983 src/cmd/ucbmail/tty.c
 r--r-----1204/70   2624 Jun 11 01:19 1983 src/cmd/ucbmail/vars.c
 r--r-----1204/70    979 Jun 11 01:19 1983 src/cmd/ucbmail/errlst.c
 r--r-----1204/70    295 Jun 11 01:19 1983 src/cmd/ucbmail/rcv.h
 r--r-----1204/70   7376 Jun 11 01:19 1983 src/cmd/ucbmail/def.h
 r--r-----1204/70   2754 Jun 11 01:19 1983 src/cmd/ucbmail/glob.h
 r--r-----1204/70    384 Jun 11 01:19 1983 src/cmd/ucbmail/local.h
 r--r-----1204/70    978 Jun 11 01:19 1983 src/cmd/ucbmail/v7.local.h
 r--r-----1204/70    159 Jun 11 01:19 1983 src/cmd/ucbmail/version
 r--r-----1204/70     29 Jun 11 01:19 1983 src/cmd/ucbmail/rofix.ed
 r--r-----1204/70    280 Jun 11 01:19 1983 src/cmd/ucbmail/versfix.ed
 rw-r-----1204/70     33 Jun 11 01:31 1983 src/cmd/ucbmail/version.c
 rw-r-----1204/70   6069 Feb 28 15:19 1983 src/cmd/ucbmail/.O
 r--r-----1204/70    893 Jun 14 19:33 1983 src/cmd/ucbmail/Mail.help
 r--r-----1204/70    656 Jun 14 19:33 1983 src/cmd/ucbmail/Mail.help.~
 rwxr-xr-x1204/70      0 Dec 15 16:56 1983 src/cmd/cpp/
 rw-r-----1204/70   3385 Dec  5 20:27 1983 src/cmd/cpp/makefile
 r--r-----1204/70  32966 Jun 10 21:35 1983 src/cmd/cpp/cpp.c
 r--r-----1204/70   1303 Jun 10 21:37 1983 src/cmd/cpp/cpy.y
 rwxr-xr-x1204/70    232 Dec  6 16:31 1983 src/cmd/cpp/:yyfix
 rwxr-xr-x1204/70    215 Dec  6 16:29 1983 src/cmd/cpp/:rofix
 r--r-----1204/70   2656 Jun 10 21:37 1983 src/cmd/cpp/yylex.c
 rwxr-xr-x1204/70    647 Nov 15 19:46 1983 src/cmd/makewhatis.sh
 rwxr-xr-x1204/70      0 Dec 15 16:58 1983 src/libc/
 rwxr-xr-x1204/70      0 Dec 15 16:56 1983 src/libc/gen/
 rw-r--r--1204/70   1786 Sep 15 16:15 1983 src/libc/gen/makefile
 rw-r--r--1204/70    350 Jun 10 18:23 1983 src/libc/gen/epdtoadr.s
 rw-r--r--1204/70    429 Jun 10 18:19 1983 src/libc/gen/atoi.c
 rw-r--r--1204/70    426 Jun 10 18:19 1983 src/libc/gen/atol.c
 rw-r--r--1204/70    604 Jun 10 18:19 1983 src/libc/gen/calloc.c
 rw-r--r--1204/70   8920 Jun 10 18:20 1983 src/libc/gen/chrtab.c
 rw-r--r--1204/70   1563 Jun 10 18:20 1983 src/libc/gen/cnlist.c
 rw-r--r--1204/70   7743 Jun 10 18:21 1983 src/libc/gen/crypt.c
 rw-r--r--1204/70   4719 Jun 10 18:22 1983 src/libc/gen/ctime.c
 rw-r--r--1204/70    763 Jun 10 18:22 1983 src/libc/gen/ctype_.c
 rw-r--r--1204/70    722 Jun 10 18:23 1983 src/libc/gen/frexp.s
 rw-r--r--1204/70    972 Jun 10 18:23 1983 src/libc/gen/errlst.c
 rw-r--r--1204/70   1580 Jun 10 18:23 1983 src/libc/gen/execvp.c
 rw-r--r--1204/70    486 Jun 10 18:32 1983 src/libc/gen/fstrcat.s
 rw-r--r--1204/70    871 Jun 10 18:23 1983 src/libc/gen/getenv.c
 rw-r--r--1204/70    634 Jun 10 18:24 1983 src/libc/gen/getlogin.c
 rw-r--r--1204/70    371 Jun 10 18:24 1983 src/libc/gen/index.c
 rw-r--r--1204/70    299 Jun 10 18:24 1983 src/libc/gen/isatty.c
 rw-r--r--1204/70    430 Jun 10 18:24 1983 src/libc/gen/l3tol.c
 rw-r--r--1204/70   2113 Jun 10 18:26 1983 src/libc/gen/modf.s
 rw-r--r--1204/70    420 Jun 10 18:24 1983 src/libc/gen/ltol3.c
 rw-r--r--1204/70   4309 Jun 10 18:25 1983 src/libc/gen/malloc.c
 rw-r--r--1204/70    460 Jun 10 18:25 1983 src/libc/gen/mktemp.c
 rw-r--r--1204/70   1093 Jun 10 18:25 1983 src/libc/gen/mon.c
 rw-r--r--1204/70  11285 Jun 10 18:26 1983 src/libc/gen/nlist.c
 rw-r--r--1204/70    500 Jun 10 18:27 1983 src/libc/gen/perror.c


                                 433010137-001                     Page 47 of 93


                                  GENIX SOURCE


 rw-r--r--1204/70   1596 Jun 10 18:27 1983 src/libc/gen/qsort.c
 rw-r--r--1204/70    283 Jun 10 18:27 1983 src/libc/gen/rand.c
 rw-r--r--1204/70   8041 Jun 10 18:28 1983 src/libc/gen/regex.c
 rw-r--r--1204/70    402 Jun 10 18:28 1983 src/libc/gen/rindex.c
 rw-r--r--1204/70    657 Jun 10 18:29 1983 src/libc/gen/sleep.c
 rw-r--r--1204/70    273 Aug 27 18:07 1983 src/libc/gen/udiv.s
 rw-r--r--1204/70    396 Jun 10 18:29 1983 src/libc/gen/strcat.c
 rw-r--r--1204/70    493 Jun 10 18:29 1983 src/libc/gen/strcatn.c
 rw-r--r--1204/70    336 Jun 10 18:30 1983 src/libc/gen/strcmp.c
 rw-r--r--1204/70    396 Jun 10 18:30 1983 src/libc/gen/strcmpn.c
 rw-r--r--1204/70    350 Jun 10 18:30 1983 src/libc/gen/strcpy.c
 rw-r--r--1204/70    474 Jun 10 18:30 1983 src/libc/gen/strcpyn.c
 rw-r--r--1204/70    312 Jun 10 18:30 1983 src/libc/gen/strlen.c
 rw-r--r--1204/70    493 Jun 10 18:30 1983 src/libc/gen/strncat.c
 rw-r--r--1204/70    396 Jun 10 18:30 1983 src/libc/gen/strncmp.c
 rw-r--r--1204/70    474 Jun 10 18:31 1983 src/libc/gen/strncpy.c
 rw-r--r--1204/70    366 Jun 10 18:31 1983 src/libc/gen/stty.c
 rw-r--r--1204/70    383 Jun 10 18:31 1983 src/libc/gen/swab.c
 rw-r--r--1204/70    242 Jun 10 18:31 1983 src/libc/gen/tell.c
 rw-r--r--1204/70   1099 Jun 10 18:31 1983 src/libc/gen/timezone.c
 rw-r--r--1204/70   1064 Jun 10 18:31 1983 src/libc/gen/ttyname.c
 rw-r--r--1204/70   1109 Jun 10 18:32 1983 src/libc/gen/ttyslot.c
 rw-r--r--1204/70    330 Jun 10 18:32 1983 src/libc/gen/valloc.c
 rw-r--r--1204/70    533 Jun 10 18:32 1983 src/libc/gen/fstrcmp.s
 rw-r--r--1204/70    423 Jun 10 18:32 1983 src/libc/gen/fstrcpy.s
 rw-r--r--1204/70    226 Sep  7 11:51 1982 src/libc/gen/README
 rw-r--r--1204/70    417 Jun 10 18:32 1983 src/libc/gen/fstrlen.s
 rw-r--r--1204/70   1314 Jun 10 18:24 1983 src/libc/gen/ldexp.s
 rw-r--r--1204/70  10193 Jun 10 18:19 1983 src/libc/gen/atof.s
 rwxr-xr-x1204/70      0 Dec 15 16:57 1983 src/libc/stdio/
 rw-r--r--1204/70   1413 Aug 27 18:15 1983 src/libc/stdio/makefile
 rw-r--r--1204/70    254 Jun 13 12:58 1983 src/libc/stdio/clrerr.c
 rw-r--r--1204/70    451 Jun 13 12:58 1983 src/libc/stdio/data.c
 rw-r--r--1204/70   4677 Jun 13 12:58 1983 src/libc/stdio/doscan.c
 rw-r--r--1204/70    295 Jun 13 12:58 1983 src/libc/stdio/error.c
 rw-r--r--1204/70    817 Jun 13 12:58 1983 src/libc/stdio/fdopen.c
 rw-r--r--1204/70    215 Jun 13 12:58 1983 src/libc/stdio/fgetc.c
 rw-r--r--1204/70    414 Jun 13 12:58 1983 src/libc/stdio/fgets.c
 rw-r--r--1204/70   1029 Jun 13 12:58 1983 src/libc/stdio/filbuf.c
 rw-r--r--1204/70   2356 Jun 13 12:58 1983 src/libc/stdio/flsbuf.c
 rw-r--r--1204/70   1053 Jun 13 12:59 1983 src/libc/stdio/fopen.c
 rw-r--r--1204/70    283 Jun 13 12:59 1983 src/libc/stdio/fprintf.c
 rw-r--r--1204/70    221 Jun 13 12:59 1983 src/libc/stdio/fputc.c
 rw-r--r--1204/70    305 Jun 13 12:59 1983 src/libc/stdio/fputs.c
 rw-r--r--1204/70    912 Jun 13 12:59 1983 src/libc/stdio/freopen.c
 rw-r--r--1204/70   1170 Jun 13 12:59 1983 src/libc/stdio/fseek.c
 rw-r--r--1204/70   2378 Jun 13 12:59 1983 src/libc/stdio/fstab.c
 rw-r--r--1204/70    669 Jun 13 12:59 1983 src/libc/stdio/ftell.c
 rw-r--r--1204/70   1160 Jun 13 12:59 1983 src/libc/stdio/gcvt.c
 rw-r--r--1204/70    281 Jun 13 12:59 1983 src/libc/stdio/getchar.c
 rw-r--r--1204/70   1102 Jun 13 12:59 1983 src/libc/stdio/getgrent.c
 rw-r--r--1204/70    364 Jun 13 12:59 1983 src/libc/stdio/getgrgid.c
 rw-r--r--1204/70    379 Jun 13 12:59 1983 src/libc/stdio/getgrnam.c
 rw-r--r--1204/70    929 Jun 13 12:59 1983 src/libc/stdio/getpass.c
 rw-r--r--1204/70    700 Jun 13 12:59 1983 src/libc/stdio/getpw.c
 rw-r--r--1204/70   1165 Jun 13 12:59 1983 src/libc/stdio/getpwent.c
 rw-r--r--1204/70    373 Jun 13 12:59 1983 src/libc/stdio/getpwnam.c
 rw-r--r--1204/70    367 Jun 13 12:59 1983 src/libc/stdio/getpwuid.c


                                 433010137-001                     Page 48 of 93


                                  GENIX SOURCE


 rw-r--r--1204/70    359 Jun 13 12:59 1983 src/libc/stdio/gets.c
 rw-r--r--1204/70    355 Jun 13 12:59 1983 src/libc/stdio/getw.c
 rw-r--r--1204/70    325 Jun 13 12:59 1983 src/libc/stdio/intss.c
 rw-r--r--1204/70   1183 Jun 13 12:59 1983 src/libc/stdio/popen.c
 rw-r--r--1204/70    270 Jun 13 12:59 1983 src/libc/stdio/printf.c
 rw-r--r--1204/70    289 Jun 13 12:59 1983 src/libc/stdio/putchar.c
 rw-r--r--1204/70    270 Jun 13 12:59 1983 src/libc/stdio/puts.c
 rw-r--r--1204/70    328 Jun 13 12:59 1983 src/libc/stdio/putw.c
 rw-r--r--1204/70    779 Jun 13 12:59 1983 src/libc/stdio/rdwr.c
 rw-r--r--1204/70    389 Jun 13 12:59 1983 src/libc/stdio/rew.c
 rw-r--r--1204/70    565 Jun 13 12:59 1983 src/libc/stdio/scanf.c
 rw-r--r--1204/70    450 Jun 13 12:59 1983 src/libc/stdio/setbuf.c
 rw-r--r--1204/70    398 Jun 13 12:59 1983 src/libc/stdio/sprintf.c
 rw-r--r--1204/70    542 Jun 13 12:59 1983 src/libc/stdio/strout.c
 rw-r--r--1204/70    197 Jun 13 12:59 1983 src/libc/stdio/stuff.c
 rw-r--r--1204/70    595 Jun 13 12:59 1983 src/libc/stdio/system.c
 rw-r--r--1204/70    255 Jun 13 12:59 1983 src/libc/stdio/tmpnam.c
 rw-r--r--1204/70    430 Jun 13 12:59 1983 src/libc/stdio/ungetc.c
 rw-r--r--1204/70  37057 Sep 26 15:44 1983 src/libc/stdio/doprnt.s
 rw-r--r--1204/70    551 Mar 23 13:46 1983 src/libc/stdio/READ_ME
 rwxr-xr-x1204/70      0 Dec 15 16:58 1983 src/libc/sys/
 rw-r--r--1204/70   1009 Jun 10 18:14 1983 src/libc/sys/setjmp.s
 rw-r--r--1204/70   1714 Sep 11 14:39 1983 src/libc/sys/makefile
 rw-r--r--1204/70    282 Jun 10 18:10 1983 src/libc/sys/abort.s
 rw-r--r--1204/70    344 Jun 10 18:10 1983 src/libc/sys/access.s
 rw-r--r--1204/70    212 Jun 10 18:10 1983 src/libc/sys/abs.s
 rw-r--r--1204/70    265 Jun 10 18:10 1983 src/libc/sys/acct.s
 rw-r--r--1204/70    331 Jun 10 18:10 1983 src/libc/sys/alarm.s
 rw-r--r--1204/70    335 Jun 10 18:10 1983 src/libc/sys/chdir.s
 rw-r--r--1204/70    317 Jun 10 18:11 1983 src/libc/sys/chmod.s
 rw-r--r--1204/70    307 Jun 10 18:11 1983 src/libc/sys/chown.s
 rw-r--r--1204/70    284 Jun 10 18:11 1983 src/libc/sys/chroot.s
 rw-r--r--1204/70    304 Jun 10 18:11 1983 src/libc/sys/close.s
 rw-r--r--1204/70    351 Jun 10 18:11 1983 src/libc/sys/creat.s
 rw-r--r--1204/70    455 Jun 10 18:11 1983 src/libc/sys/dup.s
 rw-r--r--1204/70    391 Jun 10 18:11 1983 src/libc/sys/exec.s
 rw-r--r--1204/70    431 Jun 10 18:11 1983 src/libc/sys/execl.s
 rw-r--r--1204/70    755 Jun 10 18:11 1983 src/libc/sys/execle.s
 rw-r--r--1204/70    719 Jun 10 18:11 1983 src/libc/sys/execv.s
 rw-r--r--1204/70    367 Jun 10 18:11 1983 src/libc/sys/execve.s
 rw-r--r--1204/70    313 Jun 10 18:11 1983 src/libc/sys/exit.s
 rw-r--r--1204/70    179 Jun 10 18:12 1983 src/libc/sys/fabs.s
 rw-r--r--1204/70    298 Jun 10 18:12 1983 src/libc/sys/fstat.s
 rw-r--r--1204/70    220 Jun 10 18:12 1983 src/libc/sys/getcsw.s
 rw-r--r--1204/70    511 Jun 10 18:11 1983 src/libc/sys/fork.s
 rw-r--r--1204/70    357 Jun 10 18:12 1983 src/libc/sys/getgid.s
 rw-r--r--1204/70    818 Aug 27 18:03 1983 src/libc/sys/gethostname.s
 rw-r--r--1204/70    369 Jun 10 18:12 1983 src/libc/sys/getuid.s
 rw-r--r--1204/70    318 Jun 10 18:12 1983 src/libc/sys/ioctl.s
 rw-r--r--1204/70    323 Jun 10 18:12 1983 src/libc/sys/kill.s
 rw-r--r--1204/70    305 Jun 10 18:12 1983 src/libc/sys/link.s
 rw-r--r--1204/70    329 Jun 10 18:12 1983 src/libc/sys/lseek.s
 rw-r--r--1204/70    274 Jun 10 18:12 1983 src/libc/sys/makdir.s
 rw-r--r--1204/70    242 Jun 10 18:12 1983 src/libc/sys/mdate.s
 rw-r--r--1204/70    289 Jun 10 18:12 1983 src/libc/sys/mknod.s
 rw-r--r--1204/70    277 Jun 10 18:13 1983 src/libc/sys/mount.s
 rw-r--r--1204/70    261 Jun 10 18:13 1983 src/libc/sys/nice.s
 rw-r--r--1204/70    344 Jun 10 18:13 1983 src/libc/sys/open.s


                                 433010137-001                     Page 49 of 93


                                  GENIX SOURCE


 rw-r--r--1204/70    323 Jun 10 18:13 1983 src/libc/sys/pause.s
 rw-r--r--1204/70    320 Jun 10 18:13 1983 src/libc/sys/pipe.s
 rw-r--r--1204/70    329 Jun 10 18:14 1983 src/libc/sys/ptrace.s
 rw-r--r--1204/70   2425 Jun 10 18:13 1983 src/libc/sys/prof.c
 rw-r--r--1204/70    369 Jun 10 18:14 1983 src/libc/sys/read.s
 rw-r--r--1204/70    823 Jun 10 18:14 1983 src/libc/sys/sbrk.s
 rw-r--r--1204/70    273 Jun 10 18:14 1983 src/libc/sys/setgid.s
 rw-r--r--1204/70    325 May 10 16:15 1983 src/libc/sys/sethostname.s
 rw-r--r--1204/70    273 Jun 10 18:15 1983 src/libc/sys/setuid.s
 rw-r--r--1204/70    856 Sep 11 16:49 1983 src/libc/sys/signal.s
 rw-r--r--1204/70    331 Jun 10 18:15 1983 src/libc/sys/stat.s
 rw-r--r--1204/70    251 Jun 10 18:15 1983 src/libc/sys/stime.s
 rw-r--r--1204/70    155 Jun 10 18:15 1983 src/libc/sys/sync.s
 rw-r--r--1204/70    237 Jun 10 18:15 1983 src/libc/sys/syscall.s
 rw-r--r--1204/70    573 Jun 10 18:15 1983 src/libc/sys/time.s
 rw-r--r--1204/70    233 Jun 10 18:16 1983 src/libc/sys/times.s
 rw-r--r--1204/70    274 Jun 10 18:16 1983 src/libc/sys/umask.s
 rw-r--r--1204/70    286 Jun 10 18:16 1983 src/libc/sys/umount.s
 rw-r--r--1204/70    316 Jun 10 18:16 1983 src/libc/sys/unlink.s
 rw-r--r--1204/70    278 Jun 10 18:16 1983 src/libc/sys/utime.s
 rw-r--r--1204/70   1152 Jun 10 18:16 1983 src/libc/sys/vfork.s
 rw-r--r--1204/70    277 Jun 10 18:16 1983 src/libc/sys/vhangup.s
 rw-r--r--1204/70    365 Jun 10 18:17 1983 src/libc/sys/vlimit.s
 rw-r--r--1204/70    560 Jun 10 18:17 1983 src/libc/sys/vlock.s
 rw-r--r--1204/70    255 Jun 10 18:17 1983 src/libc/sys/vmap.s
 rw-r--r--1204/70    339 Jun 10 18:17 1983 src/libc/sys/vread.s
 rw-r--r--1204/70    558 Jun 10 18:17 1983 src/libc/sys/vspy.s
 rw-r--r--1204/70    313 Jun 10 18:17 1983 src/libc/sys/vtimes.s
 rw-r--r--1204/70    330 Jun 10 18:18 1983 src/libc/sys/vwrite.s
 rw-r--r--1204/70    541 Jun 10 18:18 1983 src/libc/sys/wait.s
 rw-r--r--1204/70    359 Jun 10 18:18 1983 src/libc/sys/write.s
 rw-r--r--1204/70    302 May 11 18:09 1983 src/libc/sys/getpagesize.s
 rw-r--r--1204/70   1141 Sep 11 14:40 1983 src/libc/sys/sigentry.s
 rw-r--r--1204/70    297 Jun 10 18:12 1983 src/libc/sys/getpid.s
 rw-r--r--1204/70    265 Jun 10 18:10 1983 src/libc/sys/_exit.s
 rwxr-xr-x1204/70   1071 Sep 10 16:35 1983 src/libc/makefile
 rwxr-xr-x1204/70      0 Dec 15 16:58 1983 src/libc/csu/
 rw-r--r--1204/70    486 Aug 27 18:11 1983 src/libc/csu/makefile
 rw-r--r--1204/70   1132 Sep 11 14:32 1983 src/libc/csu/crt0.s
 rw-r--r--1204/70   2153 Sep 11 14:32 1983 src/libc/csu/mcrt0.s
 rwxr-xr-x1204/70      0 Dec 15 16:58 1983 src/libc/crt/
 rw-r--r--1204/70    490 Aug 29 14:35 1983 src/libc/crt/makefile
 rw-r--r--1204/70    346 Sep 21 09:18 1983 src/libc/crt/mcount.s
 rw-r--r--1204/70    289 Jul 29 16:35 1983 src/libc/crt/cerror.s
 rwxr-xr-x1204/70      0 Dec 15 16:59 1983 src/games/
 rwxr-xr-x1204/70   1691 Sep 14 03:03 1983 src/games/makefile
 rwxr-xr-x1204/70  14507 Jun 14 12:54 1983 src/games/backgammon.c
 rwxr-xr-x1204/70      0 Dec 15 16:58 1983 src/games/mille/
 rw-r--r--1204/70   1513 Sep  6 13:34 1983 src/games/mille/makefile
 rw-r--r--1204/70   3352 Jun 14 13:35 1983 src/games/mille/mille.h
 rw-r--r--1204/70   9806 Jun 14 13:35 1983 src/games/mille/comp.c
 rw-r--r--1204/70   2295 Jun 14 13:38 1983 src/games/mille/end.c
 rw-r--r--1204/70   3659 Jun 14 13:39 1983 src/games/mille/extern.c
 rw-r--r--1204/70   3447 Jun 14 13:41 1983 src/games/mille/init.c
 rw-r--r--1204/70   2430 Jun 14 13:45 1983 src/games/mille/mille.c
 rw-r--r--1204/70    202 Jun 14 13:46 1983 src/games/mille/unctrl.h
 rw-r--r--1204/70   3613 Jun 14 13:46 1983 src/games/mille/misc.c
 rw-r--r--1204/70   9842 Jun 14 13:47 1983 src/games/mille/move.c


                                 433010137-001                     Page 50 of 93


                                  GENIX SOURCE


 rw-r--r--1204/70   2262 Jun 14 13:51 1983 src/games/mille/print.c
 rw-r--r--1204/70    870 Jun 14 13:52 1983 src/games/mille/roll.c
 rw-r--r--1204/70   2421 Jun 14 13:52 1983 src/games/mille/save.c
 rw-r--r--1204/70    694 Jun 14 13:53 1983 src/games/mille/types.c
 rw-r--r--1204/70   1269 Jun 14 13:53 1983 src/games/mille/varpush.c
 rwxr-xr-x1204/70  72676 Jun 14 13:00 1983 src/games/banner.c
 rwxr-xr-x1204/70      0 Dec 15 16:59 1983 src/games/fortune/
 rw-r--r--1204/70   1095 Sep 14 02:47 1983 src/games/fortune/makefile
 rw-r--r--1204/70  67848 Sep 14 03:01 1983 src/games/fortune/scene
 rw-r--r--1204/70   3588 Sep 14 02:14 1983 src/games/fortune/fortune.c
 rw-r--r--1204/70  12810 Sep 14 03:00 1983 src/games/fortune/obscene
 rw-r--r--1204/70   5009 Sep 14 02:18 1983 src/games/fortune/strfile.c
 rw-r--r--1204/70    462 Sep 14 02:19 1983 src/games/fortune/strfile.h
 rw-r--r--1204/70   2090 Sep 14 02:21 1983 src/games/fortune/unstr.c
 rwxr-xr-x1204/70   1844 Jun 14 13:06 1983 src/games/bcd.c
 rwxr-xr-x1204/70   8872 Jun 14 13:06 1983 src/games/fish.c
 rwxr-xr-x1204/70  11363 Aug  1 22:01 1983 src/games/hangman.c
 rwxr-xr-x1204/70   2322 Jun 14 13:11 1983 src/games/number.c
 rwxr-xr-x1204/70      0 Dec 15 16:59 1983 src/games/adventure/
 r--r-----1204/70   4633 Aug 10 18:45 1983 src/games/adventure/vocab.c
 r--r-----1204/70   4455 Aug 10 16:03 1983 src/games/adventure/hdr.h
 r--r-----1204/70   2440 Aug 10 16:03 1983 src/games/adventure/done.c
 r--r-----1204/70   4244 Aug 10 16:08 1983 src/games/adventure/init.c
 r--r-----1204/70  14966 Aug 10 16:18 1983 src/games/adventure/io.c
 r--r-----1204/70  15469 Aug 10 16:40 1983 src/games/adventure/main.c
 rw-r-----1204/70   1783 Aug 10 16:01 1983 src/games/adventure/Makefile
 r--r-----1204/70   4597 Aug 10 18:39 1983 src/games/adventure/save.c
 r--r-----1204/70  18962 Aug 10 18:40 1983 src/games/adventure/subr.c
 rw-r-----1204/70   3618 Aug 10 14:58 1983 src/games/adventure/difflist
 r--r-----1204/70   2332 Aug 10 18:46 1983 src/games/adventure/wizard.c
 r--r-----1204/70  55605 Aug 10 18:48 1983 src/games/adventure/glorkz
 rwxr-xr-x1204/70   6459 Jun 14 13:12 1983 src/games/quiz.c
 rwxr-xr-x1204/70   3334 Jun 14 13:16 1983 src/games/rain.c
 rwxr-xr-x1204/70   4199 Jun 14 13:20 1983 src/games/worm.c
 rwxr-xr-x1204/70   8559 Jun 14 13:25 1983 src/games/worms.c
 rwxr-xr-x1204/70   7180 Jun 14 13:32 1983 src/games/wump.c
 rwxr-xr-x1204/70      0 Dec 15 17:02 1983 src/lib/
 rwxr-xr-x1204/70      0 Dec 15 16:59 1983 src/lib/libtermlib/
 rw-r-----1204/70    913 Jul 29 11:40 1983 src/lib/libtermlib/makefile
 r--r-----1204/70   6893 Jun 10 18:00 1983 src/lib/libtermlib/termcap.c
 r--r-----1204/70   3682 Jun 10 18:02 1983 src/lib/libtermlib/tgoto.c
 r--r-----1204/70   1907 Jun 10 18:02 1983 src/lib/libtermlib/tputs.c
 rwxr-xr-x1204/70      0 Dec 15 17:00 1983 src/lib/libcurses/
 rw-r--r--1204/70   2823 Dec 10 00:19 1983 src/lib/libcurses/makefile
 r--r-----1204/70   4328 Jul 13 14:54 1983 src/lib/libcurses/curses.h
 r--r-----1204/70    228 Jul 13 14:55 1983 src/lib/libcurses/unctrl.h
 r--r-----1204/70    684 Jun 10 17:03 1983 src/lib/libcurses/cr_ex.h
 r--r-----1204/70    456 Jun 10 17:03 1983 src/lib/libcurses/curses.ext
 r--r-----1204/70    747 Jun 10 17:03 1983 src/lib/libcurses/box.c
 r--r-----1204/70    511 Jun 10 17:03 1983 src/lib/libcurses/clear.c
 r--r-----1204/70   1148 Jun 10 17:04 1983 src/lib/libcurses/initscr.c
 r--r-----1204/70    387 Jun 10 17:04 1983 src/lib/libcurses/endwin.c
 r--r-----1204/70    590 Jun 10 17:05 1983 src/lib/libcurses/mvprintw.c
 r--r-----1204/70    591 Jun 10 17:05 1983 src/lib/libcurses/mvscanw.c
 r--r-----1204/70    417 Jun 10 17:06 1983 src/lib/libcurses/mvwin.c
 r--r-----1204/70   3850 Jun 10 17:06 1983 src/lib/libcurses/newwin.c
 r--r-----1204/70   1064 Jun 10 17:07 1983 src/lib/libcurses/overlay.c
 r--r-----1204/70    772 Jun 10 17:07 1983 src/lib/libcurses/overwrite.c


                                 433010137-001                     Page 51 of 93


                                  GENIX SOURCE


 r--r-----1204/70   1114 Jun 10 17:08 1983 src/lib/libcurses/scanw.c
 r--r-----1204/70   2908 Jul 19 18:38 1982 src/lib/libcurses/llib-lcurses
 r--r-----1204/70   1116 Jun 10 17:07 1983 src/lib/libcurses/printw.c
 r--r-----1204/70   5091 Jun 10 17:08 1983 src/lib/libcurses/refresh.c
 r--r-----1204/70    428 Jun 10 17:09 1983 src/lib/libcurses/touchwin.c
 r--r-----1204/70    888 Jun 10 17:09 1983 src/lib/libcurses/erase.c
 r--r-----1204/70    882 Jun 10 17:10 1983 src/lib/libcurses/clrtobot.c
 r--r-----1204/70   1002 Jun 10 17:10 1983 src/lib/libcurses/clrtoeol.c
 r--r-----1204/70   5901 Jun 10 17:11 1983 src/lib/libcurses/cr_put.c
 r--r-----1204/70   3513 Jun 10 17:12 1983 src/lib/libcurses/cr_tty.c
 r--r-----1204/70    636 Jun 10 17:13 1983 src/lib/libcurses/longname.c
 r--r-----1204/70    428 Jun 10 17:13 1983 src/lib/libcurses/delwin.c
 r--r-----1204/70    887 Jun 10 17:14 1983 src/lib/libcurses/insertln.c
 r--r-----1204/70    627 Jun 10 17:14 1983 src/lib/libcurses/deleteln.c
 r--r-----1204/70    796 Jun 10 17:15 1983 src/lib/libcurses/scroll.c
 r--r-----1204/70    431 Jun 10 17:15 1983 src/lib/libcurses/getstr.c
 r--r-----1204/70    836 Jun 10 17:15 1983 src/lib/libcurses/getch.c
 r--r-----1204/70    429 Jun 10 17:16 1983 src/lib/libcurses/addstr.c
 r--r-----1204/70   1638 Jun 10 17:16 1983 src/lib/libcurses/addch.c
 r--r-----1204/70    449 Jun 10 17:16 1983 src/lib/libcurses/move.c
 r--r-----1204/70   1154 Jun 10 17:17 1983 src/lib/libcurses/curses.c
 r--r-----1204/70    966 Jun 10 17:17 1983 src/lib/libcurses/unctrl.c
 r--r-----1204/70    562 Jun 10 17:18 1983 src/lib/libcurses/standout.c
 r--r-----1204/70    503 Jun 10 17:18 1983 src/lib/libcurses/tstp.c
 r--r-----1204/70    875 Jun 10 17:19 1983 src/lib/libcurses/insch.c
 r--r-----1204/70    705 Jun 10 17:20 1983 src/lib/libcurses/delch.c
 rwxr-xr-x1204/70      0 Dec 15 17:01 1983 src/lib/libPW/
 rwxr-x---1204/70   2110 Dec  5 15:25 1983 src/lib/libPW/makefile
 r--r-----1204/70    440 Nov 15 16:04 1983 src/lib/libPW/anystr.c
 r--r-----1204/70   1026 Nov 15 16:04 1983 src/lib/libPW/bal.c
 r--r-----1204/70   3010 Nov 15 16:05 1983 src/lib/libPW/curdir.c
 r--r-----1204/70    752 Nov 15 16:05 1983 src/lib/libPW/fdfopen.c
 r--r-----1204/70    393 Nov 15 16:05 1983 src/lib/libPW/giveup.c
 r--r-----1204/70    367 Nov 15 16:05 1983 src/lib/libPW/imatch.c
 r--r-----1204/70    701 Nov 15 16:05 1983 src/lib/libPW/PWindex.c
 r--r-----1204/70   2985 Nov 15 16:06 1983 src/lib/libPW/lockit.c
 r--r-----1204/70    204 Nov 15 16:06 1983 src/lib/libPW/logname.c
 r--r-----1204/70    461 Nov 15 16:06 1983 src/lib/libPW/move.s
 r--r-----1204/70    430 Nov 15 16:06 1983 src/lib/libPW/patoi.c
 r--r-----1204/70    428 Nov 15 16:06 1983 src/lib/libPW/patol.c
 r--r-----1204/70   3980 Nov 15 16:06 1983 src/lib/libPW/regcmp.c
 r--r-----1204/70   5808 Nov 15 16:07 1983 src/lib/libPW/regex.c
 r--r-----1204/70    475 Nov 15 16:08 1983 src/lib/libPW/rename.c
 r--r-----1204/70    440 Nov 15 16:08 1983 src/lib/libPW/repeat.c
 r--r-----1204/70    308 Nov 15 16:08 1983 src/lib/libPW/repl.c
 r--r-----1204/70    358 Nov 15 16:08 1983 src/lib/libPW/satoi.c
 r--r-----1204/70   1583 Nov 15 16:08 1983 src/lib/libPW/setsig.c
 r--r-----1204/70    701 Nov 15 16:09 1983 src/lib/libPW/sname.c
 r--r-----1204/70    218 Nov 15 16:09 1983 src/lib/libPW/strend.c
 r--r-----1204/70    724 Nov 15 16:09 1983 src/lib/libPW/substr.c
 r--r-----1204/70    545 Nov 15 16:09 1983 src/lib/libPW/trnslat.c
 r--r-----1204/70    865 Nov 15 16:09 1983 src/lib/libPW/userdir.c
 r--r-----1204/70    726 Nov 15 16:09 1983 src/lib/libPW/username.c
 r--r-----1204/70    462 Nov 15 16:10 1983 src/lib/libPW/verify.c
 r--r-----1204/70    296 Nov 15 16:10 1983 src/lib/libPW/any.c
 r--r-----1204/70   3671 Nov 15 16:10 1983 src/lib/libPW/xalloc.c
 r--r-----1204/70    856 Nov 15 16:10 1983 src/lib/libPW/xcreat.c
 r--r-----1204/70    591 Nov 15 16:10 1983 src/lib/libPW/xlink.c


                                 433010137-001                     Page 52 of 93


                                  GENIX SOURCE


 r--r-----1204/70    756 Nov 15 16:11 1983 src/lib/libPW/xopen.c
 r--r-----1204/70    355 Nov 15 16:11 1983 src/lib/libPW/xpipe.c
 r--r-----1204/70    334 Nov 15 16:11 1983 src/lib/libPW/xunlink.c
 r--r-----1204/70    473 Nov 15 16:11 1983 src/lib/libPW/xwrite.c
 r--r-----1204/70   1305 Nov 15 16:11 1983 src/lib/libPW/xmsg.c
 r--r-----1204/70    421 Nov 15 16:11 1983 src/lib/libPW/cat.c
 r--r-----1204/70    612 Nov 15 16:11 1983 src/lib/libPW/dname.c
 r--r-----1204/70   2570 Nov 15 16:12 1983 src/lib/libPW/fatal.c
 r--r-----1204/70    293 Nov 15 16:12 1983 src/lib/libPW/clean.c
 r--r-----1204/70    299 Nov 15 16:12 1983 src/lib/libPW/userexit.c
 r--r-----1204/70    327 Nov 15 16:12 1983 src/lib/libPW/zero.c
 r--r-----1204/70    325 Nov 15 16:12 1983 src/lib/libPW/zeropad.c
 rwxr-xr-x1204/70      0 Dec 15 17:01 1983 src/lib/libjobs/
 r--r-----1204/70    764 Aug 27 18:20 1983 src/lib/libjobs/makefile
 r--r-----1204/70   1528 Jun 10 17:29 1983 src/lib/libjobs/getwd.c
 r--r-----1204/70    684 Jun 10 17:30 1983 src/lib/libjobs/setpgrp.s
 r--r-----1204/70   7034 Sep 11 16:31 1983 src/lib/libjobs/signal.s
 r--r-----1204/70    912 Jun 10 17:31 1983 src/lib/libjobs/wait3.s
 r--r-----1204/70   3543 Sep 11 14:24 1983 src/lib/libjobs/sigset.c
 r--r-----1204/70    676 Jun 10 17:30 1983 src/lib/libjobs/killpg.s
 rwxr-xr-x1204/70      0 Dec 15 17:01 1983 src/lib/libln/
 r--r-----1204/70    738 Jul 14 15:42 1983 src/lib/libln/makefile
 r--r-----1204/70    664 Jun 10 17:32 1983 src/lib/libln/allprint.c
 r--r-----1204/70    195 Jun 10 17:32 1983 src/lib/libln/main.c
 r--r-----1204/70    983 Jun 10 17:32 1983 src/lib/libln/reject.c
 r--r-----1204/70    459 Jun 10 17:33 1983 src/lib/libln/yyless.c
 r--r-----1204/70    176 Jun 10 17:33 1983 src/lib/libln/yywrap.c
 rwxr-xr-x1204/70      0 Dec 15 17:01 1983 src/lib/libndir/
 r--r-----1204/70   3917 Aug 23 15:06 1983 src/lib/libndir/walkdir.c
 r--r-----1204/70   2896 Jun 10 17:47 1983 src/lib/libndir/dir.h
 r--r-----1204/70    328 Jun 10 17:47 1983 src/lib/libndir/closedir.c
 rwxr-x---1204/70   1412 Dec  5 17:43 1983 src/lib/libndir/makefile
 r--r-----1204/70    571 Jun 10 17:48 1983 src/lib/libndir/opendir.c
 r--r-----1204/70   1079 Jun 10 17:48 1983 src/lib/libndir/readdir.c
 r--r-----1204/70    654 Jun 10 17:48 1983 src/lib/libndir/seekdir.c
 r--r-----1204/70    327 Jun 10 17:48 1983 src/lib/libndir/telldir.c
 r--r-----1204/70    262 Jun 10 19:05 1983 src/lib/libndir/README
 r--r-----1204/70   2151 Jul  4 12:23 1983 src/lib/libndir/scandir.c
 rwxr-xr-x1204/70      0 Dec 15 17:02 1983 src/lib/libm/
 r--r-----1204/70    859 Aug 27 18:21 1983 src/lib/libm/makefile
 r--r-----1204/70    238 Jun 10 17:34 1983 src/lib/libm/setfsr.s
 r--r-----1204/70    788 Jun 10 17:34 1983 src/lib/libm/asin.c
 r--r-----1204/70   2341 Jun 10 17:35 1983 src/lib/libm/atan.c
 r--r-----1204/70    563 Jun 10 17:35 1983 src/lib/libm/hypot.c
 r--r-----1204/70   1825 Jun 10 17:35 1983 src/lib/libm/jn.c
 r--r-----1204/70   4419 Jun 10 17:36 1983 src/lib/libm/j0.c
 r--r-----1204/70   4489 Jun 10 17:37 1983 src/lib/libm/j1.c
 r--r-----1204/70    565 Jun 10 17:38 1983 src/lib/libm/pow.c
 r--r-----1204/70   2194 Jun 10 17:38 1983 src/lib/libm/gamma.c
 r--r-----1204/70   1232 Jun 10 17:39 1983 src/lib/libm/log.c
 r--r-----1204/70   1483 Jun 10 17:39 1983 src/lib/libm/sin.c
 r--r-----1204/70    825 Jun 10 17:39 1983 src/lib/libm/sqrt.c
 r--r-----1204/70   1448 Jun 10 17:39 1983 src/lib/libm/tan.c
 r--r-----1204/70    497 Jun 10 17:40 1983 src/lib/libm/tanh.c
 r--r-----1204/70   1442 Jun 10 17:40 1983 src/lib/libm/sinh.c
 r--r-----1204/70   2501 Jun 10 17:40 1983 src/lib/libm/erf.c
 r--r-----1204/70   1163 Jun 10 17:41 1983 src/lib/libm/exp.c
 r--r-----1204/70    430 Jun 10 17:41 1983 src/lib/libm/floor.c


                                 433010137-001                     Page 53 of 93


                                  GENIX SOURCE


 r--r-----1204/70    145 Jun 10 17:34 1983 src/lib/libm/getfsr.s
 rwxr-xr-x1204/70    782 Nov 15 17:54 1983 src/lib/makefile
 rwxr-xr-x1204/70      0 Dec 15 17:02 1983 src/lib/lib/
 rwxr-x---1204/70    531 Dec  5 15:35 1983 src/lib/lib/makefile
 r--r-----1204/70   2422 Nov 15 15:39 1983 src/lib/lib/lib.b
 rwxr-xr-x1204/70      0 Dec 15 17:02 1983 src/lib/libmp/
 r--r-----1204/70    702 Jul 14 15:51 1983 src/lib/libmp/makefile
 r--r-----1204/70    772 Jun 10 17:42 1983 src/lib/libmp/pow.c
 r--r-----1204/70    919 Jun 10 17:42 1983 src/lib/libmp/gcd.c
 r--r-----1204/70    802 Jun 10 17:42 1983 src/lib/libmp/msqrt.c
 r--r-----1204/70   2128 Jun 10 17:43 1983 src/lib/libmp/mdiv.c
 r--r-----1204/70   2175 Jun 10 17:44 1983 src/lib/libmp/mout.c
 r--r-----1204/70   1516 Jun 10 17:45 1983 src/lib/libmp/mult.c
 r--r-----1204/70   2443 Jun 10 17:46 1983 src/lib/libmp/madd.c
 r--r-----1204/70   1486 Jun 10 17:47 1983 src/lib/libmp/util.c
 rwxr-xr-x1204/70      0 Dec 15 17:02 1983 src/lib/libnsc/
 rwxr-x---1204/70   1160 Dec  5 17:47 1983 src/lib/libnsc/makefile
 r--r-----1204/70   1359 Jul 15 12:42 1983 src/lib/libnsc/fopenp.c
 r--r-----1204/70   1296 Jul 15 12:42 1983 src/lib/libnsc/openp.c
 rwxr-xr-x1204/70      0 Dec 15 17:02 1983 src/lib/libdbm/
 r--r-----1204/70    595 Aug 31 15:40 1983 src/lib/libdbm/makefile
 r--r-----1204/70   7932 Jul 14 17:11 1983 src/lib/libdbm/dbm.c
 rwxr-xr-x1204/70      0 Dec 15 17:03 1983 src/lib/libplot/
 r--r-----1204/70    788 Nov 15 17:55 1983 src/lib/libplot/makefile
 rwxr-xr-x1204/70      0 Dec 15 17:02 1983 src/lib/libplot/plot/
 r--r-----1204/70    757 Nov  8 12:25 1983 src/lib/libplot/plot/makefile
 r--r-----1204/70    284 Jun  7 16:23 1983 src/lib/libplot/plot/arc.c
 r--r-----1204/70    262 Jun  7 16:23 1983 src/lib/libplot/plot/box.c
 r--r-----1204/70    242 Jun  7 16:24 1983 src/lib/libplot/plot/circle.c
 r--r-----1204/70    231 Jun  7 16:24 1983 src/lib/libplot/plot/close.c
 r--r-----1204/70    227 Jun  7 16:24 1983 src/lib/libplot/plot/cont.c
 r--r-----1204/70    311 Jun  7 16:24 1983 src/lib/libplot/plot/dot.c
 r--r-----1204/70    201 Jun  7 16:24 1983 src/lib/libplot/plot/erase.c
 r--r-----1204/70    276 Jun  7 16:24 1983 src/lib/libplot/plot/label.c
 r--r-----1204/70    257 Jun  7 16:24 1983 src/lib/libplot/plot/line.c
 r--r-----1204/70    280 Jun  7 16:24 1983 src/lib/libplot/plot/linmod.c
 r--r-----1204/70    227 Jun  7 16:24 1983 src/lib/libplot/plot/move.c
 r--r-----1204/70    174 Jun  7 16:24 1983 src/lib/libplot/plot/open.c
 r--r-----1204/70    230 Jun  7 16:24 1983 src/lib/libplot/plot/point.c
 r--r-----1204/70    234 Jun  7 16:24 1983 src/lib/libplot/plot/putsi.c
 r--r-----1204/70    260 Jun  7 16:24 1983 src/lib/libplot/plot/space.c
 rwxr-xr-x1204/70      0 Dec 15 17:02 1983 src/lib/libplot/t300/
 rwxr-x---1204/70    742 Dec  5 15:23 1983 src/lib/libplot/t300/makefile
 rw-r-----1204/70   8169 Nov  9 14:56 1983 src/lib/libplot/t300/subr.s
 r--r-----1204/70    157 Jun  7 16:24 1983 src/lib/libplot/t300/arc.c
 r--r-----1204/70    262 Jun  7 16:24 1983 src/lib/libplot/t300/box.c
 r--r-----1204/70    166 Jun  7 16:24 1983 src/lib/libplot/t300/circle.c
 r--r-----1204/70    236 Jun  7 16:24 1983 src/lib/libplot/t300/close.c
 r--r-----1204/70    760 Jun  7 16:24 1983 src/lib/libplot/t300/con.h
 r--r-----1204/70    157 Jun  7 16:24 1983 src/lib/libplot/t300/dot.c
 r--r-----1204/70    254 Jun  7 16:24 1983 src/lib/libplot/t300/erase.c
 r--r-----1204/70    270 Jun  7 16:24 1983 src/lib/libplot/t300/label.c
 r--r-----1204/70    942 Jun  7 16:24 1983 src/lib/libplot/t300/line.c
 r--r-----1204/70    167 Jun  7 16:24 1983 src/lib/libplot/t300/linmod.c
 r--r-----1204/70    215 Jun  7 16:24 1983 src/lib/libplot/t300/move.c
 r--r-----1204/70   1126 Jun  7 16:24 1983 src/lib/libplot/t300/open.c
 r--r-----1204/70    207 Jun  7 16:24 1983 src/lib/libplot/t300/point.c
 r--r-----1204/70    342 Jun  7 16:24 1983 src/lib/libplot/t300/space.c


                                 433010137-001                     Page 54 of 93


                                  GENIX SOURCE


 r--r-----1204/70   1622 Jun  7 16:24 1983 src/lib/libplot/t300/subr.c
 rwxr-xr-x1204/70      0 Dec 15 17:02 1983 src/lib/libplot/t300s/
 r--r-----1204/70    753 Nov  8 12:35 1983 src/lib/libplot/t300s/makefile
 r--r-----1204/70    157 Jun  9 16:04 1983 src/lib/libplot/t300s/arc.c
 r--r-----1204/70    262 Jun  9 16:04 1983 src/lib/libplot/t300s/box.c
 r--r-----1204/70    166 Jun  9 16:04 1983 src/lib/libplot/t300s/circle.c
 r--r-----1204/70    236 Jun  9 16:04 1983 src/lib/libplot/t300s/close.c
 r--r-----1204/70    901 Jun  9 16:04 1983 src/lib/libplot/t300s/con.h
 r--r-----1204/70    157 Jun  9 16:04 1983 src/lib/libplot/t300s/dot.c
 r--r-----1204/70    254 Jun  9 16:04 1983 src/lib/libplot/t300s/erase.c
 r--r-----1204/70    276 Jun  9 16:04 1983 src/lib/libplot/t300s/label.c
 r--r-----1204/70    867 Jun  9 16:04 1983 src/lib/libplot/t300s/line.c
 r--r-----1204/70    167 Jun  9 16:04 1983 src/lib/libplot/t300s/linmod.c
 r--r-----1204/70    215 Jun  9 16:04 1983 src/lib/libplot/t300s/move.c
 r--r-----1204/70   1126 Jun  9 16:04 1983 src/lib/libplot/t300s/open.c
 r--r-----1204/70    224 Jun  9 16:04 1983 src/lib/libplot/t300s/point.c
 r--r-----1204/70    342 Jun  9 16:04 1983 src/lib/libplot/t300s/space.c
 r--r-----1204/70   2125 Jun  9 16:04 1983 src/lib/libplot/t300s/subr.c
 rwxr-xr-x1204/70      0 Dec 15 17:03 1983 src/lib/libplot/t4014/
 r--r-----1204/70   1990 Jun  7 18:29 1983 src/lib/libplot/t4014/arc.c
 r--r-----1204/70    761 Nov  8 12:27 1983 src/lib/libplot/t4014/makefile
 r--r-----1204/70    262 Jun  7 18:29 1983 src/lib/libplot/t4014/box.c
 r--r-----1204/70    194 Jun  7 18:29 1983 src/lib/libplot/t4014/circle.c
 r--r-----1204/70    257 Jun  7 18:29 1983 src/lib/libplot/t4014/close.c
 r--r-----1204/70    157 Jun  7 18:29 1983 src/lib/libplot/t4014/dot.c
 r--r-----1204/70    344 Jun  7 18:29 1983 src/lib/libplot/t4014/erase.c
 r--r-----1204/70    742 Jun  7 18:29 1983 src/lib/libplot/t4014/label.c
 r--r-----1204/70    199 Jun  7 18:29 1983 src/lib/libplot/t4014/line.c
 r--r-----1204/70    372 Jun  7 18:29 1983 src/lib/libplot/t4014/linemod.c
 r--r-----1204/70    192 Jun  7 18:29 1983 src/lib/libplot/t4014/move.c
 r--r-----1204/70    176 Jun  7 18:29 1983 src/lib/libplot/t4014/open.c
 r--r-----1204/70    196 Jun  7 18:29 1983 src/lib/libplot/t4014/point.c
 r--r-----1204/70    419 Jun  7 18:29 1983 src/lib/libplot/t4014/scale.c
 r--r-----1204/70    441 Jun  7 18:29 1983 src/lib/libplot/t4014/space.c
 r--r-----1204/70   1050 Jun  7 18:29 1983 src/lib/libplot/t4014/subr.c
 rwxr-xr-x1204/70      0 Dec 15 17:03 1983 src/lib/libplot/t450/
 r--r-----1204/70    157 Apr 28 11:26 1983 src/lib/libplot/t450/arc.c
 r--r-----1204/70    262 Apr 28 11:26 1983 src/lib/libplot/t450/box.c
 r--r-----1204/70    166 Apr 28 11:27 1983 src/lib/libplot/t450/circle.c
 r--r-----1204/70    236 Apr 28 11:27 1983 src/lib/libplot/t450/close.c
 r--r-----1204/70    799 Apr 28 11:28 1983 src/lib/libplot/t450/con.h
 r--r-----1204/70    157 Apr 28 11:28 1983 src/lib/libplot/t450/dot.c
 r--r-----1204/70    254 Apr 28 11:29 1983 src/lib/libplot/t450/erase.c
 r--r-----1204/70    270 Apr 28 11:29 1983 src/lib/libplot/t450/label.c
 r--r-----1204/70    944 Apr 28 11:29 1983 src/lib/libplot/t450/line.c
 r--r-----1204/70    167 Apr 28 11:30 1983 src/lib/libplot/t450/linmod.c
 r--r-----1204/70    215 Apr 28 11:30 1983 src/lib/libplot/t450/move.c
 r--r-----1204/70   1185 Apr 28 11:30 1983 src/lib/libplot/t450/open.c
 r--r-----1204/70    207 Apr 28 11:31 1983 src/lib/libplot/t450/point.c
 r--r-----1204/70    300 Apr 28 11:31 1983 src/lib/libplot/t450/space.c
 r--r-----1204/70   1653 Apr 28 11:31 1983 src/lib/libplot/t450/subr.c
 rwxr-x---1204/70    748 Dec  5 15:23 1983 src/lib/libplot/t450/makefile
 rwxr-xr-x1204/70      0 Dec 15 17:03 1983 src/lib/libplot/vt0/
 r--r-----1204/70    241 Apr 28 11:36 1983 src/lib/libplot/vt0/arc.c
 r--r-----1204/70    262 Apr 28 11:36 1983 src/lib/libplot/vt0/box.c
 r--r-----1204/70    236 Apr 28 11:36 1983 src/lib/libplot/vt0/circle.c
 r--r-----1204/70    216 Apr 28 11:36 1983 src/lib/libplot/vt0/close.c
 r--r-----1204/70    345 Apr 28 11:37 1983 src/lib/libplot/vt0/dot.c


                                 433010137-001                     Page 55 of 93


                                  GENIX SOURCE


 r--r-----1204/70    210 Apr 28 11:37 1983 src/lib/libplot/vt0/erase.c
 r--r-----1204/70    215 Apr 28 11:37 1983 src/lib/libplot/vt0/frame.c
 r--r-----1204/70    274 Apr 28 11:38 1983 src/lib/libplot/vt0/label.c
 r--r-----1204/70    410 Apr 28 11:38 1983 src/lib/libplot/vt0/line.c
 r--r-----1204/70    178 Apr 28 11:38 1983 src/lib/libplot/vt0/linmod.c
 r--r-----1204/70    305 Apr 28 11:38 1983 src/lib/libplot/vt0/move.c
 r--r-----1204/70    380 Apr 28 11:39 1983 src/lib/libplot/vt0/open.c
 r--r-----1204/70    309 Apr 28 11:39 1983 src/lib/libplot/vt0/point.c
 r--r-----1204/70    439 Apr 28 11:39 1983 src/lib/libplot/vt0/space.c
 r--r-----1204/70    396 Apr 28 11:39 1983 src/lib/libplot/vt0/subr.c
 r--r-----1204/70    747 Nov  8 12:31 1983 src/lib/libplot/vt0/makefile
 rwxr-xr-x1204/70      0 Dec 15 17:03 1983 src/db16k/
 rwxr-xr-x1204/70   1459 Sep 22 15:19 1983 src/db16k/makefile
 rwxr-xr-x1204/70    485 Sep 22 15:22 1983 src/db16k/newvers.sh
 rwxr-xr-x1204/70  42284 Sep 22 15:22 1983 src/db16k/rom.s
 rwxr-xr-x1204/70   1200 Sep 22 15:28 1983 src/db16k/prf.c
 rwxr-xr-x1204/70    499 Sep 22 15:29 1983 src/db16k/gets.c
 rwxr-xr-x1204/70   9403 Sep 22 15:30 1983 src/db16k/pio.p
 rwxr-xr-x1204/70    333 Sep 22 15:33 1983 src/db16k/sio.s
 rwxr-xr-x1204/70    490 Sep 22 15:44 1983 src/db16k/README
 rwxr-xr-x1204/70    735 Sep 23 10:27 1983 src/db16k/make2

 rwxr-xr-x1204/70      0 Dec 15 18:02 1983 usr/
 rwxr-xr-x1204/70      0 Dec 15 17:05 1983 usr/include/
 rwxr-xr-x1204/70   1594 Nov 14 15:07 1983 usr/include/makefile
 rwxr-xr-x1204/70   2457 Nov 15 15:24 1983 usr/include/a.out.h
 rwxr-xr-x1204/70    302 Nov 15 15:24 1983 usr/include/ar.h
 rwxr-xr-x1204/70    407 Nov 15 15:24 1983 usr/include/assert.h
 rwxr-xr-x1204/70    950 Nov 15 15:24 1983 usr/include/cnlist.h
 rwxr-xr-x1204/70    778 Nov 15 15:24 1983 usr/include/ctype.h
 rwxr-xr-x1204/70    544 Nov 15 15:24 1983 usr/include/dbm.h
 rwxr-xr-x1204/70    819 Nov 15 15:24 1983 usr/include/dumprestor.h
 rwxr-xr-x1204/70    730 Nov 15 15:24 1983 usr/include/errno.h
 rwxr-xr-x1204/70   1133 Nov 15 15:24 1983 usr/include/fstab.h
 rwxr-xr-x1204/70   1252 Nov 15 15:24 1983 usr/include/fsr.h
 rwxr-xr-x1204/70    194 Nov 15 15:24 1983 usr/include/grp.h
 rwxr-xr-x1204/70    128 Nov 15 15:24 1983 usr/include/ident.h
 rwxr-xr-x1204/70    150 Nov 15 15:24 1983 usr/include/lastlog.h
 rwxr-xr-x1204/70    457 Nov 15 15:24 1983 usr/include/math.h
 rwxr-xr-x1204/70    799 Nov 15 15:24 1983 usr/include/meld3.h
 rwxr-xr-x1204/70    611 Nov 15 15:24 1983 usr/include/mp.h
 rwxr-xr-x1204/70    202 Nov 15 15:24 1983 usr/include/mtab.h
 rwxr-xr-x1204/70   2063 Nov 15 15:24 1983 usr/include/nlist.h
 rwxr-xr-x1204/70    282 Nov 15 15:24 1983 usr/include/pagsiz.h
 rwxr-xr-x1204/70    275 Nov 15 15:24 1983 usr/include/pwd.h
 rwxr-xr-x1204/70   1028 Nov 15 15:24 1983 usr/include/ranlib.h
 rwxr-xr-x1204/70    954 Nov 15 15:24 1983 usr/include/saio.h
 rwxr-xr-x1204/70    363 Nov 15 15:24 1983 usr/include/walkdir.h
 rwxr-xr-x1204/70    142 Nov 15 15:24 1983 usr/include/sccs.h
 rwxr-xr-x1204/70    119 Nov 15 15:25 1983 usr/include/setjmp.h
 rwxr-xr-x1204/70   1290 Nov 15 15:25 1983 usr/include/sgtty.h
 rwxr-xr-x1204/70   2198 Nov 15 15:25 1983 usr/include/signal.h
 rwxr-xr-x1204/70   8948 Nov 15 15:25 1983 usr/include/stab.h
 rwxr-xr-x1204/70    988 Nov 15 15:25 1983 usr/include/stdio.h
 rwxr-xr-x1204/70   2820 Nov 15 15:25 1983 usr/include/sysexits.h
 rwxr-xr-x1204/70    250 Nov 15 15:25 1983 usr/include/time.h
 rwxr-xr-x1204/70    289 Nov 15 15:25 1983 usr/include/utmp.h
 rwxr-xr-x1204/70    114 Nov 15 15:25 1983 usr/include/valign.h


                                 433010137-001                     Page 56 of 93


                                  GENIX SOURCE


 rwxr-xr-x1204/70    284 Nov 15 15:25 1983 usr/include/varargs.h
 rwxr-xr-x1204/70   2294 Nov 15 15:25 1983 usr/include/wait.h
 rwxr-xr-x1204/70      4 Nov 15 15:25 1983 usr/include/whoami
 rwxr-xr-x1204/70    128 Nov 15 15:25 1983 usr/include/whoami.h
 rwxr-xr-x1204/70   1370 Nov 15 15:25 1983 usr/include/vfont.h
 rwxr-xr-x1204/70   4328 Nov 15 15:25 1983 usr/include/curses.h
 rwxr-xr-x1204/70    228 Nov 15 15:25 1983 usr/include/unctrl.h
 rwxr-xr-x1204/70   2257 Nov 15 15:25 1983 usr/include/macros.h
 rwxr-xr-x1204/70    473 Nov 15 15:25 1983 usr/include/fatal.h
 rwxr-xr-x1204/70   2854 Nov 15 15:25 1983 usr/include/dir.h
 rwxr-xr-x1204/70      0 Dec 15 17:05 1983 usr/include/sys/
 rwxr-xr-x1204/70   5277 Nov 15 15:25 1983 usr/include/sys/dcusaio.h
 rwxr-xr-x1204/70   1123 Sep 16 18:20 1983 usr/include/sys/makefile
 rwxr-xr-x1204/70    609 Nov 15 15:25 1983 usr/include/sys/ieeio.h
 rwxr-xr-x1204/70    963 Nov 15 15:25 1983 usr/include/sys/saio.h
 rwxr-xr-x1204/70    870 Nov 15 15:25 1983 usr/include/sys/acct.h
 rwxr-xr-x1204/70    478 Nov 15 15:25 1983 usr/include/sys/blt.h
 rwxr-xr-x1204/70   4227 Nov 15 15:25 1983 usr/include/sys/bootop.h
 rwxr-xr-x1204/70   3981 Nov 15 15:26 1983 usr/include/sys/buf.h
 rwxr-xr-x1204/70    584 Nov 15 15:26 1983 usr/include/sys/callout.h
 rwxr-xr-x1204/70    341 Nov 15 15:26 1983 usr/include/sys/clist.h
 rwxr-xr-x1204/70    892 Nov 15 15:26 1983 usr/include/sys/conf.h
 rwxr-xr-x1204/70   7578 Nov 15 15:26 1983 usr/include/sys/dcu.h
 rwxr-xr-x1204/70   1000 Nov 15 15:26 1983 usr/include/sys/devpm.h
 rwxr-xr-x1204/70   6336 Nov 15 15:26 1983 usr/include/sys/devvm.h
 rwxr-xr-x1204/70    186 Nov 15 15:26 1983 usr/include/sys/dir.h
 rwxr-xr-x1204/70   2318 Nov 15 15:26 1983 usr/include/sys/disk.h
 rwxr-xr-x1204/70    474 Nov 15 15:26 1983 usr/include/sys/dk.h
 rwxr-xr-x1204/70    155 Nov 15 15:26 1983 usr/include/sys/fblk.h
 rwxr-xr-x1204/70    784 Nov 15 15:26 1983 usr/include/sys/file.h
 rwxr-xr-x1204/70   1231 Nov 15 15:26 1983 usr/include/sys/filsys.h
 rwxr-xr-x1204/70    317 Nov 15 15:26 1983 usr/include/sys/fpu.h
 rwxr-xr-x1204/70   5201 Nov 15 15:26 1983 usr/include/sys/icu.h
 rwxr-xr-x1204/70    723 Nov 15 15:26 1983 usr/include/sys/ino.h
 rwxr-xr-x1204/70   2157 Nov 15 15:26 1983 usr/include/sys/inode.h
 rwxr-xr-x1204/70   4326 Nov 15 15:26 1983 usr/include/sys/ioctl.h
 rwxr-xr-x1204/70   2034 Nov 15 15:26 1983 usr/include/sys/mem.h
 rwxr-xr-x1204/70   2317 Nov 15 15:26 1983 usr/include/sys/mmu.h
 rwxr-xr-x1204/70    304 Nov 15 15:26 1983 usr/include/sys/modtable.h
 rwxr-xr-x1204/70    513 Nov 15 15:26 1983 usr/include/sys/mount.h
 rwxr-xr-x1204/70    291 Nov 15 15:26 1983 usr/include/sys/msgbuf.h
 rwxr-xr-x1204/70   2169 Nov 15 15:26 1983 usr/include/sys/mtio.h
 rwxr-xr-x1204/70   1387 Nov 15 15:26 1983 usr/include/sys/panic.h
 rwxr-xr-x1204/70   4046 Nov 15 15:26 1983 usr/include/sys/param.h
 rwxr-xr-x1204/70    184 Nov 15 15:26 1983 usr/include/sys/pdma.h
 rwxr-xr-x1204/70   4180 Nov 15 15:26 1983 usr/include/sys/proc.h
 rwxr-xr-x1204/70    931 Nov 15 15:26 1983 usr/include/sys/psr.h
 rwxr-xr-x1204/70   4842 Nov 15 15:26 1983 usr/include/sys/pte.h
 rwxr-xr-x1204/70    570 Nov 15 15:27 1983 usr/include/sys/reboot.h
 rwxr-xr-x1204/70    443 Nov 15 15:27 1983 usr/include/sys/reg.h
 rwxr-xr-x1204/70   1760 Nov 15 15:27 1983 usr/include/sys/rs.h
 rwxr-xr-x1204/70   5581 Nov 15 15:27 1983 usr/include/sys/sio.h
 rwxr-xr-x1204/70    871 Nov 15 15:27 1983 usr/include/sys/stat.h
 rwxr-xr-x1204/70   2050 Nov 15 15:27 1983 usr/include/sys/systm.h
 rwxr-xr-x1204/70   6262 Nov 15 15:27 1983 usr/include/sys/tcu.h
 rwxr-xr-x1204/70    233 Nov 15 15:27 1983 usr/include/sys/timeb.h
 rwxr-xr-x1204/70   1900 Nov 15 15:27 1983 usr/include/sys/timer.h
 rwxr-xr-x1204/70    319 Nov 15 15:27 1983 usr/include/sys/times.h


                                 433010137-001                     Page 57 of 93


                                  GENIX SOURCE


 rwxr-xr-x1204/70    390 Nov 15 15:27 1983 usr/include/sys/trap.h
 rwxr-xr-x1204/70   4326 Nov 15 15:27 1983 usr/include/sys/tty.h
 rwxr-xr-x1204/70   1131 Nov 15 15:27 1983 usr/include/sys/types.h
 rwxr-xr-x1204/70   5115 Nov 15 15:27 1983 usr/include/sys/user.h
 rwxr-xr-x1204/70    560 Nov 15 15:27 1983 usr/include/sys/vlimit.h
 rwxr-xr-x1204/70   6629 Nov 15 15:27 1983 usr/include/sys/vm.h
 rwxr-xr-x1204/70   1788 Nov 15 15:27 1983 usr/include/sys/vmmeter.h
 rwxr-xr-x1204/70   1095 Nov 15 15:27 1983 usr/include/sys/vmswap.h
 rwxr-xr-x1204/70    208 Nov 15 15:27 1983 usr/include/sys/vmsystm.h
 rwxr-xr-x1204/70    872 Nov 15 15:27 1983 usr/include/sys/vmtune.h
 rwxr-xr-x1204/70    918 Nov 15 15:27 1983 usr/include/sys/vtimes.h
 rwxr-xr-x1204/70   2369 Oct 25 17:51 1983 usr/include/sys/README
 rwxr-xr-x1204/70      0 Dec 15 17:05 1983 usr/include/local/
 rwxr-xr-x1204/70    320 Nov 15 15:25 1983 usr/include/local/uparm.h
 rwxr-xr-x1204/70    342 Mar 15 15:05 1983 usr/include/local/makefile
 rwxr-xr-x1204/70      0 Dec 15 17:05 1983 usr/bin/
 rwxr-xr-x1204/70      0 Dec 15 17:19 1983 usr/nsc/
 rwxr-xr-x1204/70      0 Dec 15 17:05 1983 usr/nsc/lib/
 rwxr-xr-x1204/70      0 Dec 15 17:05 1983 usr/nsc/lib/help/
 rwxrwxrwx1004/70     47 Aug 13 12:24 1982 usr/nsc/dir
 rwxr-xr-x1204/70      0 Dec 15 17:05 1983 usr/ucb/
 rwxr-xr-x1204/70      0 Dec 15 17:18 1983 usr/games/
 rwxr-xr-x1204/70      0 Dec 15 17:05 1983 usr/games/lib/
 rwxr-xr-x1204/70  81961 Nov 16 18:53 1983 usr/games/lib/fortunes.dat
 rwxr-xr-x1204/70      0 Dec 15 17:05 1983 usr/games/lib/quiz.k/
 rw-r-----1204/70    917 May 13 01:03 1979 usr/games/lib/quiz.k/africa
 rw-r-----1204/70    548 Jan 10 12:18 1979 usr/games/lib/quiz.k/america
 rw-r-----1204/70   3898 Jul  1 16:37 1981 usr/games/lib/quiz.k/areas
 rw-r-----1204/70    789 Jan 10 12:18 1979 usr/games/lib/quiz.k/arith
 rw-r-----1204/70    733 Jan 10 12:18 1979 usr/games/lib/quiz.k/asia
 rw-r-----1204/70    401 Jan 10 12:18 1979 usr/games/lib/quiz.k/babies
 rw-r-----1204/70   6853 Jan 10 12:18 1979 usr/games/lib/quiz.k/bard
 rw-r-----1204/70    138 Jan 10 12:18 1979 usr/games/lib/quiz.k/chinese
 rw-r-----1204/70   1403 Jan 10 12:18 1979 usr/games/lib/quiz.k/collectives
 rw-r-----1204/70   3630 Jan 19 14:09 1979 usr/games/lib/quiz.k/ed
 rw-r-----1204/70   2130 Jan 10 12:19 1979 usr/games/lib/quiz.k/elements
 rw-r-----1204/70    752 Jan 10 12:19 1979 usr/games/lib/quiz.k/europe
 rw-r-----1204/70    249 Jan 10 12:19 1979 usr/games/lib/quiz.k/greek
 rw-r-----1204/70    301 Jan 10 12:19 1979 usr/games/lib/quiz.k/inca
 rw-r-----1204/70   1550 Jul  2 02:33 1981 usr/games/lib/quiz.k/index
 rw-r-----1204/70   2956 Jan 10 12:19 1979 usr/games/lib/quiz.k/latin
 rw-r-----1204/70    163 Jan 10 12:19 1979 usr/games/lib/quiz.k/locomotive
 rw-r-----1204/70    274 Jan 10 12:19 1979 usr/games/lib/quiz.k/midearth
 rw-r-----1204/70    160 Jan 10 12:19 1979 usr/games/lib/quiz.k/morse
 rw-r-----1204/70    930 Jan 10 12:19 1979 usr/games/lib/quiz.k/murders
 rw-r-----1204/70   5967 Jan 10 12:19 1979 usr/games/lib/quiz.k/poetry
 rw-r-----1204/70    814 Jan 10 12:19 1979 usr/games/lib/quiz.k/posneg
 rw-r-----1204/70   2351 Jan 10 12:19 1979 usr/games/lib/quiz.k/pres
 rw-r-----1204/70    314 Jan 10 12:19 1979 usr/games/lib/quiz.k/province
 rw-r-----1204/70    722 Jan 10 12:19 1979 usr/games/lib/quiz.k/seq-easy
 rw-r-----1204/70    872 Jan 10 12:19 1979 usr/games/lib/quiz.k/seq-hard
 rw-r-----1204/70    405 Jan 10 12:19 1979 usr/games/lib/quiz.k/sexes
 rw-r-----1204/70   1652 Jan 10 12:19 1979 usr/games/lib/quiz.k/sov
 rw-r-----1204/70     74 Jan 10 12:19 1979 usr/games/lib/quiz.k/spell
 rw-r-----1204/70   2098 May 13 01:00 1979 usr/games/lib/quiz.k/state
 rw-r-----1204/70   1060 Jan 10 12:19 1979 usr/games/lib/quiz.k/trek
 rw-r-----1204/70   6701 Jan 10 12:19 1979 usr/games/lib/quiz.k/ucc
 rw-r--r--1204/70      0 Jul  1 12:26 1983 usr/games/lib/rogue_roll


                                 433010137-001                     Page 58 of 93


                                  GENIX SOURCE


 rwxrwx---1119/70 218768 Jun  7 17:53 1983 usr/games/rogue
 rwxr-xr-x 12/70 124200 Oct 11 22:23 1982 usr/games/adventure
 rw-rw----1056/70  23556 Aug 22 17:43 1983 usr/games/cave
 rwxr-xr-x1204/70      0 Dec 15 17:05 1983 usr/dict/
 rwxr-xr-x1204/70  50000 Dec 10 09:04 1983 usr/dict/hlista
 rwxr-xr-x1204/70  50000 Dec 10 09:04 1983 usr/dict/hlistb
 rwxr-xr-x1204/70  50000 Dec 10 09:04 1983 usr/dict/hstop
 rwxr-xr-x1204/70      0 Dec  8 10:38 1983 usr/dict/spellhist
 rwxr-xr-x1204/70 201032 Dec  8 10:38 1983 usr/dict/words
 rwxr-xr-x1204/70      0 Dec 15 17:05 1983 usr/src/
 rwxr-xr-x1204/70      0 Dec 15 17:54 1983 usr/src/db16k/
 rwxr-xr-x1204/70    490 Dec 10 09:16 1983 usr/src/db16k/README
 rwxr-xr-x1204/70    735 Dec 10 09:16 1983 usr/src/db16k/makefile
 rwxr-xr-x1204/70    485 Dec 10 09:16 1983 usr/src/db16k/newvers.sh
 rwxr-xr-x1204/70   9403 Dec 10 09:16 1983 usr/src/db16k/pio.p
 rwxr-xr-x1204/70   9057 Dec 10 09:16 1983 usr/src/db16k/pio.s
 rwxr-xr-x1204/70  42403 Dec 10 09:16 1983 usr/src/db16k/rom.V
 rwxr-xr-x1204/70  42284 Dec 10 09:16 1983 usr/src/db16k/rom.s
 rwxr-xr-x1204/70    333 Dec 10 09:16 1983 usr/src/db16k/sio.s
 rwxr-xr-x1204/70    119 Dec 10 09:16 1983 usr/src/db16k/vers.s
 rwxr-xr-x1204/70      2 Dec 10 09:16 1983 usr/src/db16k/version
 rwxr-xr-x1204/70      0 Dec 15 17:19 1983 usr/lib/
 rwxr-xr-x1204/70      0 Dec 15 17:06 1983 usr/lib/cref/
 rwxr-xr-x1204/70      0 Dec 15 17:06 1983 usr/lib/font/
 rwxr-xr-x1204/70      0 Dec 15 17:06 1983 usr/lib/lint/
 rwxr-xr-x1204/70      0 Dec 15 17:06 1983 usr/lib/tabset/
 rwxr-xr-x1204/70      0 Dec 15 17:06 1983 usr/lib/term/
 rwxr-xr-x1204/70      0 Dec 15 17:06 1983 usr/lib/lex/
 rwxr-x---1204/70   4009 Nov 16 18:20 1983 usr/lib/lex/ncform
 rwxr-xr-x1204/70      0 Dec 15 17:06 1983 usr/lib/tmac/
 rwxr-xr-x1204/70    438 Dec 15 17:30 1983 usr/lib/tmac/makefile
 rwxr-xr-x1204/70   7726 Jun  7 18:31 1983 usr/lib/tmac/tmac.a
 rwxr-xr-x1204/70   4313 Sep  8 18:55 1983 usr/lib/tmac/tmac.an
 rw-r--r--  0/70  12056 Dec 11 04:18 1983 usr/lib/tmac/tmac.e
 rwxr-xr-x1204/70   2226 Jun  7 18:31 1983 usr/lib/tmac/tmac.cp
 rwxr-xr-x1204/70    163 Jun  7 18:31 1983 usr/lib/tmac/tmac.m
 rwxr-xr-x1204/70   1349 Jun  7 18:31 1983 usr/lib/tmac/tmac.r
 rwxr-xr-x1204/70  23324 Jun  7 18:31 1983 usr/lib/tmac/tmac.s
 rwxr-xr-x1204/70   4398 Jun  7 18:31 1983 usr/lib/tmac/tmac.scover
 rwxr-xr-x1204/70    906 Jun  7 18:31 1983 usr/lib/tmac/tmac.sdisp
 rwxr-xr-x1204/70   1287 Jun  7 18:31 1983 usr/lib/tmac/tmac.skeep
 rwxr-xr-x1204/70   2201 Jun  7 18:31 1983 usr/lib/tmac/tmac.srefs
 rwxr-xr-x1204/70    133 Jun  7 18:31 1983 usr/lib/tmac/tmac.vcat
 rwxr-xr-x1204/70    776 Jun  7 18:31 1983 usr/lib/tmac/tmac.vgrind
 rwxr-xr-x1204/70      0 Dec 15 17:06 1983 usr/lib/macros/
 rwxr-xr-x1204/70  37824 May 26 16:54 1983 usr/lib/macros/mmn
 rwxr-xr-x1204/70  38397 May 26 16:55 1983 usr/lib/macros/mmt
 rwxr-xr-x1204/70    350 Jun  7 12:20 1983 usr/lib/macros/makefile
 rwxr-xr-x1204/70      0 Dec 15 17:06 1983 usr/lib/uucp/
 rwxr-xr-x1204/70    151 Aug 22 15:59 1983 usr/lib/uucp/L.sys
 rwxr-xr-x1204/70     50 Aug 22 15:51 1983 usr/lib/uucp/L-devices
 rwxr-xr-x1204/70     13 Aug 22 15:58 1983 usr/lib/uucp/L-dialcodes
 rwxr-xr-x1204/70     12 Aug 22 15:54 1983 usr/lib/uucp/USERFILE
 rwxr-xr-x1204/70    299 Aug 22 15:57 1983 usr/lib/uucp/README
 rwxr-xr-x1204/70      0 Aug 25 13:19 1983 usr/lib/uucp/SEQF
 rwxr-xr-x1204/70      0 Dec 15 17:06 1983 usr/lib/me/
 rwxr-xr-x1204/70      0 Dec 15 17:06 1983 usr/lib/me/src/
 rwxr-xr-x1204/70    671 Jun  7 17:39 1983 usr/lib/me/src/makefile


                                 433010137-001                     Page 59 of 93


                                  GENIX SOURCE


 rwxr-xr-x1204/70    699 Jun  7 11:51 1983 usr/lib/me/src/acm.me
 rwxr-xr-x1204/70   1717 Jun  7 11:51 1983 usr/lib/me/src/chars.me
 rwxr-xr-x1204/70    364 Jun  7 11:51 1983 usr/lib/me/src/deltext.me
 rwxr-xr-x1204/70   1171 Jun  7 11:51 1983 usr/lib/me/src/eqn.me
 rwxr-xr-x1204/70   1047 Jun  7 11:51 1983 usr/lib/me/src/float.me
 rwxr-xr-x1204/70   1380 Jun  7 11:51 1983 usr/lib/me/src/footnote.me
 rwxr-xr-x1204/70   1027 Jun  7 11:51 1983 usr/lib/me/src/index.me
 rwxr-xr-x1204/70    451 Jun  7 11:51 1983 usr/lib/me/src/local.me
 rwxr-xr-x1204/70    272 Jun  7 11:51 1983 usr/lib/me/src/null.me
 rwxr-xr-x1204/70   1781 Jun  7 11:51 1983 usr/lib/me/src/sh.me
 rwxr-xr-x1204/70   1101 Jun  7 11:51 1983 usr/lib/me/src/tbl.me
 rwxr-xr-x1204/70    593 Jun  7 11:51 1983 usr/lib/me/src/thesis.me
 rwxr-xr-x1204/70  19203 Jun  7 11:51 1983 usr/lib/me/src/tmac.e
 rwxr-xr-x1204/70    714 Jun  7 11:51 1983 usr/lib/me/src/install.csh
 rwxr-xr-x1204/70    174 Jun  7 11:51 1983 usr/lib/me/src/test.accent
 rwxr-xr-x1204/70    415 Jun  7 11:51 1983 usr/lib/me/src/test.umlaut
 rwxr-xr-x1204/70   4607 Jun  7 11:51 1983 usr/lib/me/src/revisions
 rwxr-xr-x1204/70    738 Dec  8 23:16 1983 usr/lib/me/acm.me
 rwxr-xr-x1204/70   1546 Dec  8 23:16 1983 usr/lib/me/chars.me
 rwxr-xr-x1204/70    344 Dec  8 23:16 1983 usr/lib/me/deltext.me
 rwxr-xr-x1204/70    976 Dec  8 23:16 1983 usr/lib/me/eqn.me
 rwxr-xr-x1204/70    831 Dec  8 23:16 1983 usr/lib/me/float.me
 rwxr-xr-x1204/70   1132 Dec  8 23:16 1983 usr/lib/me/footnote.me
 rwxr-xr-x1204/70    855 Dec  8 23:16 1983 usr/lib/me/index.me
 rwxr-xr-x1204/70    513 Dec  8 23:16 1983 usr/lib/me/local.me
 rwxr-xr-x1204/70    185 Dec  8 23:16 1983 usr/lib/me/null.me
 rwxr-xr-x1204/70   1558 Dec  8 23:16 1983 usr/lib/me/sh.me
 rwxr-xr-x1204/70    976 Dec  8 23:16 1983 usr/lib/me/tbl.me
 rwxr-xr-x1204/70    528 Dec  8 23:16 1983 usr/lib/me/thesis.me
 rwxr-xr-x1204/70   4607 Dec  8 23:17 1983 usr/lib/me/revisions
 rw-rw---- 12/70     93 Sep 28 13:22 1983 usr/lib/crontab
 rw-r--r--1086/70  18499 Nov 16 18:13 1983 usr/lib/whatis
 rw-r--r--1119/70    517 Aug 25 12:53 1983 usr/lib/aliases
 rwxr-xr-x1204/70      0 Dec 15 17:06 1983 usr/spool/
 rwxr-xr-x1204/70      0 Dec 15 17:06 1983 usr/spool/uucp/
 rwxrwx---1204/70      0 Dec 15 17:06 1983 usr/spool/uucp/.XQTDIR/
 rwxr-xr-x1204/70      0 Dec 15 17:06 1983 usr/spool/uucppublic/
 r--r--r--1204/70      0 Aug  2 10:23 1983 usr/spool/uucppublic/.hushlogin
 rwxr-xr-x1204/70      0 Dec 15 17:06 1983 usr/spool/secretmail/
 rwxr-x---1204/70     31 Nov 16 18:49 1983 usr/spool/secretmail/notice
 rwxr-xr-x1204/70      0 Dec 15 17:06 1983 usr/spool/slp/
 rwxr-xr-x1204/70      0 Dec 15 17:06 1983 usr/spool/mail/
 rwxr-xr-x1204/70      0 Dec 15 17:06 1983 usr/spool/lpd/
 rwxr-xr-x1204/70      0 Dec 15 17:06 1983 usr/spool/at/
 rwxr-xr-x1204/70      0 Dec 15 17:06 1983 usr/spool/at/past/
 rwxr-xr-x1204/70      0 Dec 15 17:06 1983 usr/local/
 rw-r--r--1204/70    627 Sep 21 11:01 1983 usr/local/README
 rwxr-xr-x1204/70      0 Dec 15 17:06 1983 usr/pub/
 rw-r-----1204/70   2114 Jul 27 02:50 1982 usr/pub/ascii
 rw-r-----1204/70   2969 Jun 16 16:52 1983 usr/pub/eqnchar
 rw-r-----1204/70    475 Jun 16 16:52 1983 usr/pub/greek
 rwxr-xr-x1204/70      0 Dec 15 17:06 1983 usr/skel/
 rw-r-----1204/70    239 Sep 13 13:42 1983 usr/skel/.cshrc
 rw-r-----1204/70     53 Sep 13 13:43 1983 usr/skel/.login
 rw-r-----1204/70     37 Sep 13 13:43 1983 usr/skel/.exrc
 rw-r-----1204/70    112 Sep 13 13:44 1983 usr/skel/.plan
 rw-r-----1204/70     29 Sep 13 13:44 1983 usr/skel/.project
 rw-r-----1204/70    219 Sep 29 11:19 1983 usr/skel/README


                                 433010137-001                     Page 60 of 93


                                  GENIX SOURCE


 rw-r-----1204/70    116 Sep 13 13:48 1983 usr/skel/.mailrc
 rw-r-----1204/70      6 Sep 13 13:49 1983 usr/skel/.logout
 rwxr-xr-x1204/70      0 Dec 15 17:06 1983 usr/tmp/
 rw-r--r--1204/70      0 Sep 21 11:02 1983 usr/tmp/.hushlogin
 rwxr-xr-x1204/70      0 Dec 15 17:28 1983 usr/adm/
 rw-r--r--  0/10      0 Dec 15 17:28 1983 usr/adm/lastlog
 rw-r--r--  0/10      0 Dec 15 17:28 1983 usr/adm/messages
 rw-r--r--  0/10      0 Dec 15 17:28 1983 usr/adm/msgbuf
 rw-r--r--  0/10      0 Dec 15 17:28 1983 usr/adm/shutdownlog
 rw-r--r--  0/10      0 Dec 15 17:28 1983 usr/adm/wtmp
 rwxr-xr-x1204/70      0 Dec 15 17:06 1983 usr/crash/
 rw-r-----1204/70      2 Dec 11 19:04 1983 usr/crash/bounds
 rwxr-xr-x1204/70      0 Dec 15 17:06 1983 usr/man/
 rwxr-xr-x1204/70      0 Dec 15 17:06 1983 usr/msgs/
 rwxr-xr-x1204/70      0 Dec 15 17:06 1983 usr/preserve/

 rw-r--r--1204/0    183 Sep 20 13:53 1983 .cshrc
 rw-r--r--1204/0    116 Sep 13 17:13 1983 .login
 rwxr-xr-x1204/0      0 Dec 15 17:08 1983 bin/
 rwxr-xr-x1204/0      0 Dec 15 17:08 1983 dev/
 rwxr-xr-x1204/0      0 Dec 15 17:29 1983 etc/
 rwxr-xr-x1204/0   1949 Dec  7 14:50 1983 etc/gettytab
 rwxr-xr-x1204/0    128 Dec  7 14:50 1983 etc/group
 rwxr-xr-x1204/0   1169 Dec  7 14:50 1983 etc/group.howto
 rwxr-xr-x1204/0   1069 Dec  7 14:50 1983 etc/passwd
 rwxr-xr-x1204/0   1200 Dec  7 14:50 1983 etc/passwd.howto
 rwxr-xr-x1204/0     98 Dec  7 14:50 1983 etc/ttys
 rwxr-xr-x1204/0    158 Dec  7 14:50 1983 etc/ttytype
 rwxr-x---1204/0   1558 Nov 17 14:41 1983 etc/mkdownload
 rwxr-x---1204/0    717 Nov 16 18:51 1983 etc/mkguest
 rwxr-x---1204/0  23744 Nov 18 17:18 1983 etc/setroot
 rwxr-x---1204/0  41053 Nov 16 18:29 1983 etc/termcap
 rw-r-----1204/0     19 Aug 18 17:10 1983 etc/fstab
 rw-r--r--  0/10      0 Dec 15 17:29 1983 etc/utmp
 rwxr-xr-x1204/0      0 Dec 15 18:01 1983 lib/
 rwxr-xr-x1204/0      0 Dec 15 17:08 1983 lost+found/
 rwxr-xr-x1204/0      0 Dec 15 17:08 1983 stand/
 rwxr-xr-x1204/0      0 Dec 15 17:08 1983 sys/
 rwxr-xr-x1204/0      0 Dec 15 17:08 1983 sys/SYS16/
 rwxr-xr-x1204/0      0 Dec 15 17:08 1983 sys/dev/
 rwxr-xr-x1204/0      0 Dec 15 17:08 1983 sys/h/
 rwxr-xr-x1204/0      0 Dec 15 17:08 1983 sys/sys/
 rwxr-xr-x1204/0      0 Dec 15 17:08 1983 tmp/
 rwxr-xr-x1204/70      0 Dec 15 18:11 1983 usr/

 rwxr-xr-x1188/70      0 Dec 15 18:21 1983 NSC/
 rwxr-xr-x1188/70  36864 Dec  6 15:09 1983 NSC/nmpc
 rwxr-xr-x1188/70      0 Dec 15 18:01 1983 NSC/lib/
 rwxr-xr-x1188/70 221184 Dec  6 15:10 1983 NSC/lib/npcom
 rwxr-xr-x1188/70  40960 Dec  6 15:11 1983 NSC/lib/npmsg
 rwxr-xr-x1188/70  24986 Dec  6 15:11 1983 NSC/lib/libp.a
 rwxr-xr-x1188/70  24986 Dec  6 15:11 1983 NSC/lib/libpc.a
                                 linked to NSC/lib/libp.a
 rwxr-x---219/10  10542 Dec 13 23:25 1983 NSC/lib/libdbm.a
 rwxr-x---219/10  12066 Dec 13 23:26 1983 NSC/lib/libmp.a
 rwxr-x---219/10  31886 Dec 13 23:33 1983 NSC/lib/libPW.a
 rwxr-x---219/10   5372 Dec 13 23:33 1983 NSC/lib/libndir.a
 rwxr-xr-x1188/410   2211 Dec  2 16:45 1983 NSC/minstall


                                 433010137-001                     Page 61 of 93


                                  GENIX SOURCE


 rwxr-xr-x1188/70   8655 Dec  6 15:12 1983 NSC/mstrip
 rwxr-xr-x  0/0    671 Dec  9 18:17 1983 NSC/mmakewhatis


























































                                 433010137-001                     Page 62 of 93


                                  GENIX SOURCE


_T_a_p_e _2:  _G_E_N_I_X_1._0_A_I_I

 total 282
 drwxrwxr-x11 1056        928 Nov 22 16:38 ./
 drwxr-xr-x18 root        816 Dec 15 16:12 ../
 -rw-rw---- 1 1056        183 Sep 20 13:53 .cshrc
 -rwxrwx--- 1 1004        116 Sep 13 17:13 .login*
 drwxrwxrwx 2 1056       1040 Nov 16 18:51 bin/
 drwxrwxrwx 2 1056       1616 Sep 27 15:44 dev/
 drwxrwxrwx 2 1056        928 Nov 18 10:57 etc/
 drwxrwxrwx 2 1086        224 Nov 16 17:58 lib/
 drwxr-xr-x 2 root       4128 Sep  8 18:27 lost+found/
 drwxrwxrwx 2 1121        240 Oct 18 17:06 stand/
 drwxrwx--- 6 1121         96 Nov 16 09:20 sys/
 drwxrwxrwx 2 1056         32 May 23  1982 tmp/
 drwxr-xr-x 2 1056        336 Aug 19 13:40 usr/
 -rw-rw---- 1 1004     129285 Nov 17 18:43 vmunix

 ./bin:
 total 2061
 drwxrwxrwx 2 1056       1040 Nov 16 18:51 ./
 drwxrwxr-x11 1056        928 Nov 22 16:38 ../
 -rwxrwx--- 2 1086       5856 Nov 16 18:02 [*
 -rwxrwx--- 1 1086      19876 Nov 16 17:59 ar*
 -rwxrwx--- 1 1086      10540 Nov 16 17:52 as*
 -rwxrwx--- 1 1086      62736 Nov 16 18:13 awk*
 -rwxrwx--- 1 1086      10216 Nov 16 17:59 cat*
 -rwxrwx--- 1 1086      23590 Nov 16 17:58 cc*
 -rwxrwx--- 1 1086       9048 Nov 16 17:59 chmod*
 -rwxrwx--- 1 1086      10332 Nov 16 17:59 cmp*
 -rwxrwx--- 1 1086      10988 Nov 16 17:59 cp*
 -rwxrwx--- 1 1086      72356 Nov 16 18:14 csh*
 -rwxrwx--- 1 1056      74512 Sep 21 18:44 cshrec*
 -rwxrwx--- 1 1086      11660 Nov 16 17:59 date*
 -rwxrwx--- 1 1086      13992 Nov 16 17:59 dd*
 -rwxrwx--- 1 1086      13740 Nov 16 17:59 df*
 -rwxrwx--- 1 1086      25260 Nov 16 18:16 diff*
 -rwxrwx--- 1 1086      10212 Nov 16 17:59 du*
 -rwxrwx--- 2 1086      14972 Nov 16 18:18 e*
 -rwxrwx--- 1 1086       3668 Nov 16 17:59 echo*
 -rwxrwx--- 2 1086      14972 Nov 16 18:18 ed*
 -rwxrwx--- 1 1086      14760 Nov 16 17:59 expr*
 -rwxrwx--- 1 1086        122 Nov 16 17:59 false*
 -rwxrwx--- 1 1086      20184 Nov 16 18:51 getv*
 -rwxrwx--- 1 1086      12516 Nov 16 18:20 grep*
 -rwxrwx--- 1 1086       9832 Nov 16 18:00 hostname*
 -rwxrwx--- 1 1086      10304 Nov 16 18:00 kill*
 -rwxrwx--- 1 1086      35108 Nov 16 17:53 ld*
 -rwxrwx--- 1 1086       9936 Nov 16 18:00 ln*
 -rwxrwx--- 1 1086      23124 Nov 16 18:00 login*
 -rwxrwx--- 1 1086      14124 Nov 16 18:22 ls*
 -rwxrwx--- 1 1086      24736 Nov 16 18:00 mail*
 -rwxrwx--- 1 1086      40264 Nov 16 18:23 make*
 -rwxrwx--- 1 1086       9104 Nov 16 18:00 mkdir*
 -rwxrwx--- 1 1086      11396 Nov 16 18:00 mt*
 -rwxrwx--- 1 1086      12088 Nov 16 18:00 mv*
 -rwxrwx--- 1 1086      11044 Nov 16 18:00 nice*
 -rwxrwx--- 1 1086      18700 Nov 16 18:00 nm*


                                 433010137-001                     Page 63 of 93


                                  GENIX SOURCE


 -rwxrwx--- 1 1086        260 Nov 16 18:00 nohup*
 -rwxrwx--- 1 1086      12296 Nov 16 18:00 od*
 -rwxrwx--- 1 1086      18252 Nov 16 18:01 passwd*
 -rwxrwx--- 1 1086      13712 Nov 16 18:01 pr*
 -rwxrwx--- 1 1086      21864 Nov 16 18:01 ps*
 -rwxrwx--- 1 1086      14608 Nov 16 18:51 putv*
 -rwxrwx--- 1 1086       9008 Nov 16 18:01 pwd*
 -rwxrwx--- 1 1086      10464 Nov 16 18:01 rm*
 -rwxrwx--- 1 1086      13920 Nov 16 18:01 rmail*
 -rwxrwx--- 1 1086       9244 Nov 16 18:01 rmdir*
 -rwxrwx--- 1 1086      19280 Nov 16 18:28 sed*
 -rwxrwx--- 1 1086      30780 Nov 16 18:28 sh*
 -rwxrwx--- 1 1086       9168 Nov 16 18:01 size*
 -rwxrwx--- 1 1086       9176 Nov 16 18:01 strip*
 -rwxrwx--- 1 1086      17040 Nov 16 18:01 stty*
 -rwxrwx--- 1 1086      18456 Nov 16 18:01 su*
 -rwxrwx--- 1 1086       3632 Nov 16 18:01 sync*
 -rwxrwx--- 1 1086      24544 Nov 16 18:01 tar*
 -rwxrwx--- 1 1086       4764 Nov 16 18:02 tee*
 -rwxrwx--- 2 1086       5856 Nov 16 18:02 test*
 -rwxrwx--- 1 1086      11076 Nov 16 18:02 time*
 -rwxrwx--- 1 1086        113 Nov 16 18:02 true*
 -rwxr-x--- 1 1086      22132 Nov 16 18:22 ucbls*
 -rwxrwx--- 2 1086      12868 Nov 16 18:02 wall*
 -rwxrwx--- 1 1086      13888 Nov 16 18:02 who*
 -rwxrwx--- 1 1086      12996 Nov 16 18:02 write*

 ./dev:
 total 6
 drwxrwxrwx 2 1056       1616 Sep 27 15:44 ./
 drwxrwxr-x11 1056        928 Nov 22 16:38 ../
 crw-rw-rw- 1 root      0,  0 May 22  1982 console
 brw------- 1 root      0,  0 Jun 20 17:31 dc0a
 brw------- 1 root      0,  1 Jun 20 17:32 dc0b
 brw------- 1 root      0,  2 Jun 20 17:32 dc0c
 brw------- 1 root      0,  3 Sep 15 15:26 dc0d
 brw------- 1 root      0,  4 Sep 15 15:27 dc0e
 brw------- 1 root      0,  5 Sep 15 15:28 dc0f
 brw------- 1 root      0,  6 Jun 20 17:32 dc0g
 brw------- 1 root      0,  7 Sep 15 15:28 dc0h
 brw------- 1 root      0,  8 Jun  9  1982 dc1a
 brw------- 2 root      0,  9 Jun  9  1982 dc1b
 brw------- 1 root      0, 10 Sep 25  1982 dc1c
 brw------- 1 root      0, 11 Sep 15 15:29 dc1d
 brw------- 1 root      0, 12 Sep 15 15:29 dc1e
 brw------- 1 root      0, 13 Sep 15 15:30 dc1f
 brw------- 1 root      0, 14 Jun  7  1982 dc1g
 brw------- 1 root      0, 15 Sep 15 15:30 dc1h
 brw------- 1 root      0, 16 Sep 15 15:30 dc2a
 brw------- 1 root      0, 17 Sep 15 15:31 dc2b
 brw------- 1 root      0, 18 Sep 15 15:31 dc2c
 brw------- 1 root      0, 19 Sep 15 15:31 dc2d
 brw------- 1 root      0, 20 Sep 15 15:31 dc2e
 brw------- 1 root      0, 21 Sep 15 15:31 dc2f
 brw------- 1 root      0, 22 Sep 15 15:32 dc2g
 brw------- 1 root      0, 23 Sep 15 15:32 dc2h
 brw------- 1 root      0, 24 Sep 15 15:32 dc3a
 brw------- 1 root      0, 25 Sep 15 15:32 dc3b


                                 433010137-001                     Page 64 of 93


                                  GENIX SOURCE


 brw------- 1 root      0, 26 Sep 15 15:32 dc3c
 brw------- 1 root      0, 27 Sep 15 15:32 dc3d
 brw------- 1 root      0, 28 Sep 15 15:33 dc3e
 brw------- 1 root      0, 29 Sep 15 15:33 dc3f
 brw------- 1 root      0, 30 Sep 15 15:33 dc3g
 brw------- 1 root      0, 31 Sep 15 15:33 dc3h
 brw------- 2 root      0,  9 Jun  9  1982 drum
 crw-r--r-- 1 root      1,  1 Jun 12  1982 kmem
 crw-rw-rw- 1 root      9,  2 Sep 27 15:44 lp
 crw-r--r-- 1 root      1,  0 Jun 12  1982 mem
 crw-rw-rw- 2 root      6,  4 Nov  9  1982 nrtc0
 brw-rw-rw- 2 root      1,  4 Nov  9  1982 ntc0
 crw-rw-rw- 1 root      1,  2 Jun 12  1982 null
 crw-rw-rw- 1 root      5,  0 Nov  3  1982 pty0
 crw-rw-rw- 1 root      5,  1 Nov  5  1982 pty1
 crw-rw-rw- 1 root      5,  2 Nov  5  1982 pty2
 crw-rw-rw- 1 root      5,  3 Nov  5  1982 pty3
 crw-rw-rw- 1 31        5,  4 Nov  5  1982 pty4
 crw-rw-rw- 1 root      5,  5 Nov  5  1982 pty5
 crw------- 1 root      2,  0 Jun 20 17:32 rdc0a
 crw------- 1 root      2,  1 Jun 20 17:32 rdc0b
 crw------- 1 root      2,  2 Jun 20 17:33 rdc0c
 crw------- 1 root      2,  3 Sep 15 15:38 rdc0d
 crw------- 1 root      2,  4 Sep 15 15:38 rdc0e
 crw------- 1 root      2,  5 Sep 15 15:39 rdc0f
 crw------- 1 root      2,  6 Jun 20 17:33 rdc0g
 crw------- 1 root      2,  7 Sep 15 15:39 rdc0h
 crw------- 1 root      2,  8 Aug 21  1982 rdc1a
 crw------- 1 root      2,  9 Aug 21  1982 rdc1b
 crw------- 1 root      2, 10 Jan 20  1983 rdc1c
 crw------- 1 root      2, 11 Sep 15 15:39 rdc1d
 crw------- 1 root      2, 12 Sep 15 15:39 rdc1e
 crw------- 1 root      2, 13 Sep 15 15:40 rdc1f
 crw------- 1 root      2, 14 Aug 21  1982 rdc1g
 crw------- 1 root      2, 15 Sep 15 15:40 rdc1h
 crw------- 1 root      2, 16 Sep 15 15:40 rdc2a
 crw------- 1 root      2, 17 Sep 15 15:41 rdc2b
 crw------- 1 root      2, 18 Sep 15 15:41 rdc2c
 crw------- 1 root      2, 19 Sep 15 15:41 rdc2d
 crw------- 1 root      2, 20 Sep 15 15:41 rdc2e
 crw------- 1 root      2, 21 Sep 15 15:41 rdc2f
 crw------- 1 root      2, 22 Sep 15 15:42 rdc2g
 crw------- 1 root      2, 23 Sep 15 15:42 rdc2h
 crw------- 1 root      2, 24 Sep 15 15:42 rdc3a
 crw------- 1 root      2, 25 Sep 15 15:42 rdc3b
 crw------- 1 root      2, 26 Sep 15 15:43 rdc3c
 crw------- 1 root      2, 27 Sep 15 15:43 rdc3d
 crw------- 1 root      2, 28 Sep 15 15:43 rdc3e
 crw------- 1 root      2, 29 Sep 15 15:43 rdc3f
 crw------- 1 root      2, 30 Sep 15 15:43 rdc3g
 crw------- 1 root      2, 31 Sep 15 15:43 rdc3h
 crw-rw-rw- 1 root      6,  0 Nov  9  1982 rtc0
 crw-rw-rw- 2 root      6,  4 Nov  9  1982 rtc4
 brw-rw-rw- 1 root      1,  0 Nov  9  1982 tc0
 brw-rw-rw- 2 root      1,  4 Nov  9  1982 tc4
 crw-rw-rw- 1 root      3,  1 Nov 21 16:20 tty
 crw-rw-rw- 1 root      8,  0 Dec 27  1982 tty00
 crw-rw-rw- 1 root      8,  1 Dec 27  1982 tty01


                                 433010137-001                     Page 65 of 93


                                  GENIX SOURCE


 crw-rw-rw- 1 root      8,  2 Dec 27  1982 tty02
 crw-rw-rw- 1 root      8,  3 Dec 27  1982 tty03
 crw-rw-rw- 1 root      8,  4 Dec 27  1982 tty04
 crw-rw-rw- 1 root      8,  5 Dec 27  1982 tty05
 crw-rw-rw- 1 root      8,  6 Dec 27  1982 tty06
 crw-rw-rw- 1 root      8,  7 Dec 27  1982 tty07
 crw-rw-rw- 1 root      7,  0 Dec 27  1982 tty08
 crw-rw-rw- 1 root      4,  0 Nov  3  1982 ttyp0
 crw-rw-rw- 1 root      4,  1 Nov  5  1982 ttyp1
 crw-rw-rw- 1 root      4,  2 Nov  5  1982 ttyp2
 crw-rw-rw- 1 root      4,  3 Nov  5  1982 ttyp3
 crw-rw-rw- 1 root      4,  4 Nov  5  1982 ttyp4
 crw-rw-rw- 1 root      4,  5 Nov  5  1982 ttyp5

 ./etc:
 total 1170
 drwxrwxrwx 2 1056        928 Nov 18 10:57 ./
 drwxrwxr-x11 1056        928 Nov 22 16:38 ../
 -rwxrwx--- 1 1086      11580 Nov 16 18:02 ac*
 -rwxrwx--- 1 1086       5492 Nov 16 18:02 accton*
 -rwxrwx--- 1 1086       5524 Nov 16 18:02 badsect*
 -rwxrwx--- 1 1086      12608 Nov 16 18:02 catman*
 -rwxrwx--- 1 1086      12724 Nov 16 18:02 chgrp*
 -rwxrwx--- 1 1086      12352 Nov 16 18:02 chown*
 -rwxrwx--- 1 1086       9076 Nov 16 18:02 clri*
 -rwxrwx--- 1 1086       8400 Nov 16 18:03 cron*
 -rwxrwx--- 1 1086      10292 Nov 16 18:03 dcheck*
 -rwxrwx--- 1 1086      40840 Nov 17 11:51 delivermail*
 -rwxrwx--- 1 1086      10420 Nov 16 18:03 dmesg*
 -rwxrwx--- 1 1086      30976 Nov 16 18:18 dump*
 -rwxrwx--- 1 1086      14016 Nov 16 18:03 dumpdir*
 -rwxrwx--- 1 1086      27504 Nov 16 18:03 fsck*
 -rw-rw---- 1 1056         19 Aug 18 17:10 fstab
 -rwxrwx--- 1 1086      15016 Nov 16 18:03 getty*
 -rw-rw---- 1 1119       1949 Aug  2 09:27 gettytab
 -rw-rw---- 1 1190        128 Sep 26 19:50 group
 -rw-rw---- 1 1190       1169 Sep 26 19:49 group.howto
 -rwxrwx--- 1 1086      13748 Nov 16 18:03 halt*
 -rwxrwx--- 1 1086      11664 Nov 16 18:03 icheck*
 -rwxrwx--- 1 1086       8136 Nov 16 18:03 init*
 -rwxrwx--- 1 1086       1558 Nov 17 14:41 mkdownload*
 -rwxrwx--- 1 1086      13680 Nov 16 18:03 mkfs*
 -rwxrwx--- 1 1086        717 Nov 16 18:51 mkguest*
 -rwxrwx--- 1 1086        570 Nov 16 18:03 mklost+found*
 -rwxrwx--- 1 1086       9812 Nov 16 18:03 mknod*
 -rwxrwx--- 1 1086      12444 Nov 16 18:03 mount*
 -rwxrwx--- 1 1086      11304 Nov 16 18:04 ncheck*
 -rwxrwx--- 1 1086        212 Nov 16 18:51 newps*
 -rwxrwx--- 1 1086        369 Nov 16 18:04 newsym*
 -rw-rw-r-- 1 1190       1069 Sep 26 19:53 passwd
 -rw-rw---- 1 1190       1200 Sep 26 19:21 passwd.howto
 -rwxrwx--- 1 1086      22632 Nov 16 18:24 pstat*
 -rwxrwx--- 1 1086      19592 Nov 16 18:04 quot*
 -rwxrwx--- 1 1086        913 Nov 16 18:04 rc*
 -rwxrwx--- 1 1086      13612 Nov 16 18:04 reboot*
 -rwxrwx--- 1 1086      11140 Nov 16 18:04 renice*
 -rwxrwx--- 1 1086      18768 Nov 16 18:04 restor*
 -rwxrwx--- 1 1086      17180 Nov 16 18:04 sa*


                                 433010137-001                     Page 66 of 93


                                  GENIX SOURCE


 -rwxrwx--- 1 1056      19404 Sep 19 19:13 sash*
 -rwxrwx--- 1 1086      17672 Nov 16 18:04 savecore*
 -rwxrwx--- 1 1086      13332 Nov 16 18:51 setdate*
 -rwxrwx--- 1 1086      23744 Nov 18 17:18 setroot*
 -rwxrwx--- 1 1086      16096 Nov 16 18:04 shutdown*
 -rwxrwx--- 1 1086      41053 Nov 16 18:29 termcap*
 -rw-rw---- 1 1056         98 Nov 18 16:37 ttys
 -rw-rw---- 1 1056        158 Aug 10 12:25 ttytype
 -rwxrwx--- 1 1086      12452 Nov 16 18:04 umount*
 -rwxrwx--- 1 1086       4728 Nov 16 18:04 update*
 -rw-r--r-- 1 1056          0 Nov 30  1982 utmp
 -rwxrwx--- 1 1086        338 Nov 16 18:04 vipw*
 -rwxrwx--- 2 1086      12868 Nov 16 18:02 wall*

 ./lib:
 total 955
 drwxrwxrwx 2 1086        224 Nov 16 17:58 ./
 drwxrwxr-x11 1056        928 Nov 22 16:38 ../
 -rwxrwx--- 1 1086     107740 Nov 16 17:52 asm*
 -rwxrwx--- 1 1086      61903 Nov 16 17:52 c2*
 -rwxrwx--- 1 1086     156823 Nov 16 17:58 ccom*
 -rwxrwx--- 1 1086      31854 Nov 16 17:53 cpp*
 -rw-rw-r-- 2 1086        397 Nov 15 17:51 crt0.O
 -rw-rw-r-- 2 1086        397 Nov 15 17:51 crt0.o
 -rw-rw-r-- 1 1086     106198 Nov 15 17:51 libc.a
 -rwxrwx--- 1 1086      18100 Nov 16 17:52 list*
 -rwxrwx--- 2 1086        811 Nov 15 17:51 mcrt0.O*
 -rwxrwx--- 2 1086        811 Nov 15 17:51 mcrt0.o*

 ./lost+found:
 total 11
 drwxr-xr-x 2 root       4128 Sep  8 18:27 ./
 drwxrwxr-x11 1056        928 Nov 22 16:38 ../

 ./stand:
 total 476
 drwxrwxrwx 2 1121        240 Oct 18 17:06 ./
 drwxrwxr-x11 1056        928 Nov 22 16:38 ../
 -rwxrwx--- 1 1007      13716 Oct 18 17:05 aboot*
 -rwxrwx--- 1 1007      13780 Oct 18 17:05 boot*
 -rwxrwx--- 1 1007      13392 Oct 18 17:05 cat*
 -rwxrwx--- 1 1007      15836 Oct 18 17:05 copy*
 -rwxrwx--- 1 1007      21220 Oct 18 17:05 dcutest*
 -rwxrwx--- 1 1007      13788 Oct 18 17:05 eboot*
 -rwxrwx--- 1 1007      17072 Oct 18 17:06 icheck*
 -rwxrwx--- 1 1007      14424 Oct 18 17:06 ls*
 -rwxrwx--- 1 1007      16904 Oct 18 17:06 mkfs*
 -rwxrwx--- 1 1007      15176 Oct 18 17:06 mkpt*
 -rwxrwx--- 1 1007      17952 Oct 18 17:06 restor*
 -r-xr-x--- 1 1007      65691 Oct  4 15:22 tsc*

 ./sys:
 total 7
 drwxrwx--- 6 1121         96 Nov 16 09:20 ./
 drwxrwxr-x11 1056        928 Nov 22 16:38 ../
 drwxrwx--- 2 1121         32 Nov 16 09:20 SYS16/
 drwxrwx--- 2 1121         32 Nov 16 09:20 dev/
 drwxrwx--- 2 1121         32 Nov 16 09:20 h/


                                 433010137-001                     Page 67 of 93


                                  GENIX SOURCE


 drwxrwx--- 2 1121         32 Nov 16 09:20 sys/

 ./sys/SYS16:
 total 2
 drwxrwx--- 2 1121         32 Nov 16 09:20 ./
 drwxrwx--- 6 1121         96 Nov 16 09:20 ../

 ./sys/dev:
 total 2
 drwxrwx--- 2 1121         32 Nov 16 09:20 ./
 drwxrwx--- 6 1121         96 Nov 16 09:20 ../

 ./sys/h:
 total 2
 drwxrwx--- 2 1121         32 Nov 16 09:20 ./
 drwxrwx--- 6 1121         96 Nov 16 09:20 ../

 ./sys/sys:
 total 2
 drwxrwx--- 2 1121         32 Nov 16 09:20 ./
 drwxrwx--- 6 1121         96 Nov 16 09:20 ../

 ./tmp:
 total 3
 drwxrwxrwx 2 1056         32 May 23  1982 ./
 drwxrwxr-x11 1056        928 Nov 22 16:38 ../

 ./usr:
 rwxrwxr-x 12/70      0 Nov  8 09:42 1983 usr/
 rwxrwxr-x 12/70      0 Nov 15 15:26 1983 usr/include/
 r--r-----1086/70   1594 Nov 14 15:07 1983 usr/include/makefile
 rw-r-----1086/70   5525 Nov 15 15:27 1983 usr/include/result
 r--r-----1086/70   2457 Nov 15 15:24 1983 usr/include/a.out.h
 r--r-----1086/70    302 Nov 15 15:24 1983 usr/include/ar.h
 r--r-----1086/70    407 Nov 15 15:24 1983 usr/include/assert.h
 r--r--r--1086/70    950 Nov 15 15:24 1983 usr/include/cnlist.h
 r--r-----1086/70    778 Nov 15 15:24 1983 usr/include/ctype.h
 r--r-----1086/70    544 Nov 15 15:24 1983 usr/include/dbm.h
 r--r-----1086/70    819 Nov 15 15:24 1983 usr/include/dumprestor.h
 r--r-----1086/70    730 Nov 15 15:24 1983 usr/include/errno.h
 r--r-----1086/70   1133 Nov 15 15:24 1983 usr/include/fstab.h
 r--r-----1086/70   1252 Nov 15 15:24 1983 usr/include/fsr.h
 r--r-----1086/70    194 Nov 15 15:24 1983 usr/include/grp.h
 r--r-----1086/70    128 Nov 15 15:24 1983 usr/include/ident.h
 r--r-----1086/70    150 Nov 15 15:24 1983 usr/include/lastlog.h
 r--r-----1086/70    457 Nov 15 15:24 1983 usr/include/math.h
 r--r-----1086/70    799 Nov 15 15:24 1983 usr/include/meld3.h
 r--r-----1086/70    611 Nov 15 15:24 1983 usr/include/mp.h
 r--r-----1086/70    202 Nov 15 15:24 1983 usr/include/mtab.h
 r--r--r--1086/70   2063 Nov 15 15:24 1983 usr/include/nlist.h
 r--r-----1086/70    282 Nov 15 15:24 1983 usr/include/pagsiz.h
 r--r-----1086/70    275 Nov 15 15:24 1983 usr/include/pwd.h
 r--r-----1086/70   1028 Nov 15 15:24 1983 usr/include/ranlib.h
 r--r-----1086/70    954 Nov 15 15:24 1983 usr/include/saio.h
 r--r-----1086/70    363 Nov 15 15:24 1983 usr/include/walkdir.h
 r--r-----1086/70    142 Nov 15 15:24 1983 usr/include/sccs.h
 r--r-----1086/70    119 Nov 15 15:25 1983 usr/include/setjmp.h
 r--r-----1086/70   1290 Nov 15 15:25 1983 usr/include/sgtty.h


                                 433010137-001                     Page 68 of 93


                                  GENIX SOURCE


 r--r-----1086/70   2198 Nov 15 15:25 1983 usr/include/signal.h
 r--r-----1086/70   8948 Nov 15 15:25 1983 usr/include/stab.h
 r--r-----1086/70    988 Nov 15 15:25 1983 usr/include/stdio.h
 r--r-----1086/70   2820 Nov 15 15:25 1983 usr/include/sysexits.h
 r--r-----1086/70    250 Nov 15 15:25 1983 usr/include/time.h
 r--r-----1086/70    289 Nov 15 15:25 1983 usr/include/utmp.h
 r--r-----1086/70    114 Nov 15 15:25 1983 usr/include/valign.h
 r--r-----1086/70    284 Nov 15 15:25 1983 usr/include/varargs.h
 r--r-----1086/70   2294 Nov 15 15:25 1983 usr/include/wait.h
 r--r-----1086/70      4 Nov 15 15:25 1983 usr/include/whoami
 r--r-----1086/70    128 Nov 15 15:25 1983 usr/include/whoami.h
 r--r-----1086/70   1370 Nov 15 15:25 1983 usr/include/vfont.h
 r--r-----1086/70   4328 Nov 15 15:25 1983 usr/include/curses.h
 r--r-----1086/70    228 Nov 15 15:25 1983 usr/include/unctrl.h
 r--r-----1086/70   2257 Nov 15 15:25 1983 usr/include/macros.h
 r--r-----1086/70    473 Nov 15 15:25 1983 usr/include/fatal.h
 r--r-----1086/70   2854 Nov 15 15:25 1983 usr/include/dir.h
 rwxrwxr-x 12/70      0 Nov 15 15:27 1983 usr/include/sys/
 r--r-----1086/70   5277 Nov 15 15:25 1983 usr/include/sys/dcusaio.h
 r--r-----1007/70   1123 Sep 16 18:20 1983 usr/include/sys/makefile
 r--r-----1086/70    609 Nov 15 15:25 1983 usr/include/sys/ieeio.h
 r--r-----1086/70    963 Nov 15 15:25 1983 usr/include/sys/saio.h
 r--r-----1086/70    870 Nov 15 15:25 1983 usr/include/sys/acct.h
 r--r-----1086/70    478 Nov 15 15:25 1983 usr/include/sys/blt.h
 r--r-----1086/70   4227 Nov 15 15:25 1983 usr/include/sys/bootop.h
 r--r-----1086/70   3981 Nov 15 15:26 1983 usr/include/sys/buf.h
 r--r-----1086/70    584 Nov 15 15:26 1983 usr/include/sys/callout.h
 r--r-----1086/70    341 Nov 15 15:26 1983 usr/include/sys/clist.h
 r--r-----1086/70    892 Nov 15 15:26 1983 usr/include/sys/conf.h
 r--r-----1086/70   7578 Nov 15 15:26 1983 usr/include/sys/dcu.h
 r--r-----1086/70   1000 Nov 15 15:26 1983 usr/include/sys/devpm.h
 r--r-----1086/70   6336 Nov 15 15:26 1983 usr/include/sys/devvm.h
 r--r-----1086/70    186 Nov 15 15:26 1983 usr/include/sys/dir.h
 r--r-----1086/70   2318 Nov 15 15:26 1983 usr/include/sys/disk.h
 r--r-----1086/70    474 Nov 15 15:26 1983 usr/include/sys/dk.h
 r--r-----1086/70    155 Nov 15 15:26 1983 usr/include/sys/fblk.h
 r--r-----1086/70    784 Nov 15 15:26 1983 usr/include/sys/file.h
 r--r-----1086/70   1231 Nov 15 15:26 1983 usr/include/sys/filsys.h
 r--r-----1086/70    317 Nov 15 15:26 1983 usr/include/sys/fpu.h
 r--r-----1086/70   5201 Nov 15 15:26 1983 usr/include/sys/icu.h
 r--r-----1086/70    723 Nov 15 15:26 1983 usr/include/sys/ino.h
 r--r-----1086/70   2157 Nov 15 15:26 1983 usr/include/sys/inode.h
 r--r-----1086/70   4326 Nov 15 15:26 1983 usr/include/sys/ioctl.h
 r--r-----1086/70   2034 Nov 15 15:26 1983 usr/include/sys/mem.h
 r--r-----1086/70   2317 Nov 15 15:26 1983 usr/include/sys/mmu.h
 r--r-----1086/70    304 Nov 15 15:26 1983 usr/include/sys/modtable.h
 r--r-----1086/70    513 Nov 15 15:26 1983 usr/include/sys/mount.h
 r--r-----1086/70    291 Nov 15 15:26 1983 usr/include/sys/msgbuf.h
 r--r-----1086/70   2169 Nov 15 15:26 1983 usr/include/sys/mtio.h
 r--r-----1086/70   1387 Nov 15 15:26 1983 usr/include/sys/panic.h
 r--r-----1086/70   4046 Nov 15 15:26 1983 usr/include/sys/param.h
 r--r-----1086/70    184 Nov 15 15:26 1983 usr/include/sys/pdma.h
 r--r-----1086/70   4180 Nov 15 15:26 1983 usr/include/sys/proc.h
 r--r-----1086/70    931 Nov 15 15:26 1983 usr/include/sys/psr.h
 r--r-----1086/70   4842 Nov 15 15:26 1983 usr/include/sys/pte.h
 r--r-----1086/70    570 Nov 15 15:27 1983 usr/include/sys/reboot.h
 r--r-----1086/70    443 Nov 15 15:27 1983 usr/include/sys/reg.h
 r--r-----1086/70   1760 Nov 15 15:27 1983 usr/include/sys/rs.h


                                 433010137-001                     Page 69 of 93


                                  GENIX SOURCE


 r--r-----1086/70   5581 Nov 15 15:27 1983 usr/include/sys/sio.h
 r--r--r--1086/70    871 Nov 15 15:27 1983 usr/include/sys/stat.h
 r--r-----1086/70   2050 Nov 15 15:27 1983 usr/include/sys/systm.h
 r--r-----1086/70   6262 Nov 15 15:27 1983 usr/include/sys/tcu.h
 r--r-----1086/70    233 Nov 15 15:27 1983 usr/include/sys/timeb.h
 r--r-----1086/70   1900 Nov 15 15:27 1983 usr/include/sys/timer.h
 r--r-----1086/70    319 Nov 15 15:27 1983 usr/include/sys/times.h
 r--r-----1086/70    390 Nov 15 15:27 1983 usr/include/sys/trap.h
 r--r-----1086/70   4326 Nov 15 15:27 1983 usr/include/sys/tty.h
 r--r--r--1086/70   1131 Nov 15 15:27 1983 usr/include/sys/types.h
 r--r-----1086/70   5115 Nov 15 15:27 1983 usr/include/sys/user.h
 r--r-----1086/70    560 Nov 15 15:27 1983 usr/include/sys/vlimit.h
 r--r-----1086/70   6629 Nov 15 15:27 1983 usr/include/sys/vm.h
 r--r-----1086/70   1788 Nov 15 15:27 1983 usr/include/sys/vmmeter.h
 r--r-----1086/70   1095 Nov 15 15:27 1983 usr/include/sys/vmswap.h
 r--r-----1086/70    208 Nov 15 15:27 1983 usr/include/sys/vmsystm.h
 r--r-----1086/70    872 Nov 15 15:27 1983 usr/include/sys/vmtune.h
 r--r-----1086/70    918 Nov 15 15:27 1983 usr/include/sys/vtimes.h
 rw-r-----1197/70   2369 Oct 25 17:51 1983 usr/include/sys/README
 rwxrwxr-x 12/70      0 Nov 15 15:25 1983 usr/include/local/
 r--r-----1086/70    320 Nov 15 15:25 1983 usr/include/local/uparm.h
 r--r--r--1119/70    342 Mar 15 15:05 1983 usr/include/local/makefile
 rwxrwx---1056/70      0 May 11 16:54 1983 usr/crash/
 rw-rw----1056/70      2 May 11 16:54 1983 usr/crash/bounds
 rwxrwx---1086/70      0 Aug 19 13:11 1983 usr/src/
 rwxrwx---1086/70      0 Nov 16 18:50 1983 usr/src/db16k/
 rwxrwx---1086/70   9403 Nov 16 18:50 1983 usr/src/db16k/pio.p
 rwxrwx---1086/70    333 Nov 16 18:50 1983 usr/src/db16k/sio.s
 rwxrwx---1086/70  42284 Nov 16 18:50 1983 usr/src/db16k/rom.s
 rwxrwx---1086/70    485 Nov 16 18:50 1983 usr/src/db16k/newvers.sh
 rwxrwx---1086/70    835 Nov 16 18:50 1983 usr/src/db16k/prf.o
 rwxrwx---1086/70    450 Nov 16 18:50 1983 usr/src/db16k/gets.o
 rwxrwx---1086/70   2201 Nov 16 18:50 1983 usr/src/db16k/pio.o
 rwxrwx---1086/70    243 Nov 16 18:50 1983 usr/src/db16k/sio.o
 rwxrwx---1086/70   9821 Nov 16 18:50 1983 usr/src/db16k/rom.o
 rwxrwx---1086/70  10833 Nov 16 18:50 1983 usr/src/db16k/dbmon.x
 rwxrwx---1086/70    119 Nov 16 18:50 1983 usr/src/db16k/vers.s
 rwxrwx---1086/70      2 Nov 16 18:50 1983 usr/src/db16k/version
 rwxrwx---1086/70  42403 Nov 16 18:50 1983 usr/src/db16k/rom.V
 rwxrwx---1086/70   8978 Nov 16 18:50 1983 usr/src/db16k/pio.s
 rwxrwx---1086/70    735 Nov 16 18:50 1983 usr/src/db16k/makefile
 rwxrwx---1086/70    490 Nov 16 18:50 1983 usr/src/db16k/README
 rwxr-xr-x1190/70      0 Sep 13 16:41 1983 usr/skel/
 rw-rw----1190/70    239 Sep 13 13:42 1983 usr/skel/.cshrc
 rw-rw----1190/70     53 Sep 13 13:43 1983 usr/skel/.login
 rw-rw----1190/70     37 Sep 13 13:43 1983 usr/skel/.exrc
 rw-rw----1190/70    112 Sep 13 13:44 1983 usr/skel/.plan
 rw-rw----1190/70     29 Sep 13 13:44 1983 usr/skel/.project
 rw-rw----1190/70    219 Sep 29 11:19 1983 usr/skel/README
 rw-rw----1190/70    116 Sep 13 13:48 1983 usr/skel/.mailrc
 rw-rw----1190/70      6 Sep 13 13:49 1983 usr/skel/.logout
 rwxr-xr-x1190/70      0 Sep 21 11:02 1983 usr/local/
 rw-rw-r--1190/70    627 Sep 21 11:01 1983 usr/local/README
 rwxrwx---1056/70      0 Nov 16 18:53 1983 usr/games/
 rwxrwx---1086/70   5192 Nov 16 18:52 1983 usr/games/bcd
 rwxrwx---1086/70  13888 Nov 16 18:52 1983 usr/games/rain
 rwxrwx---1086/70  23196 Nov 16 18:53 1983 usr/games/worm
 rwxrwx---1086/70  17168 Nov 16 18:53 1983 usr/games/worms


                                 433010137-001                     Page 70 of 93


                                  GENIX SOURCE


 rwxrwx---1086/70  19064 Nov 16 18:52 1983 usr/games/backgammon
 rwxrwx--- 12/70      0 Nov 16 18:53 1983 usr/games/lib/
 rwxrwx---1056/70      0 Oct 20 23:33 1982 usr/games/lib/quiz.k/
 rw-rw----1056/70    917 May 13 01:03 1979 usr/games/lib/quiz.k/africa
 rw-rw----1056/70    548 Jan 10 12:18 1979 usr/games/lib/quiz.k/america
 rw-rw----1056/70   3898 Jul  1 16:37 1981 usr/games/lib/quiz.k/areas
 rw-rw----1056/70    789 Jan 10 12:18 1979 usr/games/lib/quiz.k/arith
 rw-rw----1056/70    733 Jan 10 12:18 1979 usr/games/lib/quiz.k/asia
 rw-rw----1056/70    401 Jan 10 12:18 1979 usr/games/lib/quiz.k/babies
 rw-rw----1056/70   6853 Jan 10 12:18 1979 usr/games/lib/quiz.k/bard
 rw-rw----1056/70    138 Jan 10 12:18 1979 usr/games/lib/quiz.k/chinese
 rw-rw----1056/70   1403 Jan 10 12:18 1979 usr/games/lib/quiz.k/collectives
 rw-rw----1056/70   3630 Jan 19 14:09 1979 usr/games/lib/quiz.k/ed
 rw-rw----1056/70   2130 Jan 10 12:19 1979 usr/games/lib/quiz.k/elements
 rw-rw----1056/70    752 Jan 10 12:19 1979 usr/games/lib/quiz.k/europe
 rw-rw----1056/70    249 Jan 10 12:19 1979 usr/games/lib/quiz.k/greek
 rw-rw----1056/70    301 Jan 10 12:19 1979 usr/games/lib/quiz.k/inca
 rw-rw----1056/70   1550 Jul  2 02:33 1981 usr/games/lib/quiz.k/index
 rw-rw----1056/70   2956 Jan 10 12:19 1979 usr/games/lib/quiz.k/latin
 rw-rw----1056/70    163 Jan 10 12:19 1979 usr/games/lib/quiz.k/locomotive
 rw-rw----1056/70    274 Jan 10 12:19 1979 usr/games/lib/quiz.k/midearth
 rw-rw----1056/70    160 Jan 10 12:19 1979 usr/games/lib/quiz.k/morse
 rw-rw----1056/70    930 Jan 10 12:19 1979 usr/games/lib/quiz.k/murders
 rw-rw----1056/70   5967 Jan 10 12:19 1979 usr/games/lib/quiz.k/poetry
 rw-rw----1056/70    814 Jan 10 12:19 1979 usr/games/lib/quiz.k/posneg
 rw-rw----1056/70   2351 Jan 10 12:19 1979 usr/games/lib/quiz.k/pres
 rw-rw----1056/70    314 Jan 10 12:19 1979 usr/games/lib/quiz.k/province
 rw-rw----1056/70    722 Jan 10 12:19 1979 usr/games/lib/quiz.k/seq-easy
 rw-rw----1056/70    872 Jan 10 12:19 1979 usr/games/lib/quiz.k/seq-hard
 rw-rw----1056/70    405 Jan 10 12:19 1979 usr/games/lib/quiz.k/sexes
 rw-rw----1056/70   1652 Jan 10 12:19 1979 usr/games/lib/quiz.k/sov
 rw-rw----1056/70     74 Jan 10 12:19 1979 usr/games/lib/quiz.k/spell
 rw-rw----1056/70   2098 May 13 01:00 1979 usr/games/lib/quiz.k/state
 rw-rw----1056/70   1060 Jan 10 12:19 1979 usr/games/lib/quiz.k/trek
 rw-rw----1056/70   6701 Jan 10 12:19 1979 usr/games/lib/quiz.k/ucc
 rw-rw-rw-1119/70      0 Jul  1 12:26 1983 usr/games/lib/rogue_roll
 rwxrwx---1086/70  81961 Nov 16 18:53 1983 usr/games/lib/fortunes.dat
 rwxrwx---1119/70 218768 Jun  7 17:53 1983 usr/games/rogue
 rwxrwx--- 12/70  11383 Nov 24 17:59 1982 usr/games/dpytest
 rwxrwx--- 12/70  12251 Sep 12 19:36 1982 usr/games/drawtest
 rwxr-xr-x 12/70 124200 Oct 11 22:23 1982 usr/games/adventure
 rwxrwx---1086/70  20196 Nov 16 18:52 1983 usr/games/banner
 rwxrwx---1086/70  12972 Nov 16 18:52 1983 usr/games/fish
 rwxrwx---1086/70  32512 Nov 16 18:52 1983 usr/games/hangman
 rwxrwx---1086/70  10768 Nov 16 18:52 1983 usr/games/number
 rwxrwx---1086/70  12568 Nov 16 18:52 1983 usr/games/quiz
 rwxrwx---1086/70  13656 Nov 16 18:53 1983 usr/games/wump
 rwxrwx---1056/70      0 Jan 12 15:02 1983 usr/games/graph/
 rw-r-----1056/70  60800 Nov  9 20:54 1981 usr/games/graph/MESA
 rw-r-----1056/70  60800 Nov  4 12:08 1981 usr/games/graph/dogfight
 rw-r-----1056/70  60800 Mar 24 09:03 1982 usr/games/graph/fonts
 rw-r-----1056/70  60800 Sep 26 10:30 1980 usr/games/graph/ise
 rw-r-----1056/70  60800 May 13 17:51 1981 usr/games/graph/karnaugh
 rw-r-----1056/70  60800 Oct 30 13:04 1981 usr/games/graph/layout
 rw-r-----1056/70  60800 Apr  7 10:13 1982 usr/games/graph/mar
 rw-r-----1056/70  60800 Nov  4 15:34 1981 usr/games/graph/mesasign
 rw-r-----1056/70  60800 Apr 12 13:45 1982 usr/games/graph/mey-sah
 rw-r-----1056/70  60800 Nov 20 15:32 1980 usr/games/graph/moire


                                 433010137-001                     Page 71 of 93


                                  GENIX SOURCE


 rw-r-----1056/70  60800 Oct 30 13:10 1981 usr/games/graph/state
 rw-r-----1056/70  60800 Mar  6 09:37 1981 usr/games/graph/timing
 rw-r-----1056/70  60800 Aug 18 18:21 1981 usr/games/graph/toaster
 rw-r-----1056/70  60800 Nov  4 09:58 1981 usr/games/graph/workstation
 rw-rw----1056/70  23556 Aug 22 17:43 1983 usr/games/cave
 rwxrwx---1086/70  43048 Nov 16 18:53 1983 usr/games/mille
 rwxrwx---1086/70  12728 Nov 16 18:53 1983 usr/games/fortune
 rwxrwxr-x1190/70      0 Sep 28 17:59 1983 usr/man/
 rwxrwxrwx1190/70      0 Sep 28 17:59 1983 usr/man/man0/
 rwxrwxrwx1190/70      0 Sep 28 17:59 1983 usr/man/man1/
 rwxrwxrwx1190/70      0 Sep 28 17:59 1983 usr/man/man2/
 rwxrwxrwx1190/70      0 Sep 28 17:59 1983 usr/man/man3/
 rwxrwxrwx1190/70      0 Sep 28 17:59 1983 usr/man/man4/
 rwxrwxrwx1190/70      0 Sep 28 17:59 1983 usr/man/man5/
 rwxrwxrwx1190/70      0 Sep 28 17:59 1983 usr/man/man6/
 rwxrwxrwx1190/70      0 Sep 28 17:59 1983 usr/man/man7/
 rwxrwxrwx1190/70      0 Sep 28 17:59 1983 usr/man/man8/
 rwxrwxrwx1190/70      0 Sep 28 17:59 1983 usr/man/manl/
 rwxrwxrwx1190/70      0 Sep 28 17:59 1983 usr/man/cat1/
 rwxrwxrwx1190/70      0 Sep 28 17:59 1983 usr/man/cat2/
 rwxrwxrwx1190/70      0 Sep 28 17:59 1983 usr/man/cat3/
 rwxrwxrwx1190/70      0 Sep 28 17:59 1983 usr/man/cat4/
 rwxrwxrwx1190/70      0 Sep 28 17:59 1983 usr/man/cat5/
 rwxrwxrwx1190/70      0 Sep 28 17:59 1983 usr/man/cat6/
 rwxrwxrwx1190/70      0 Sep 28 17:59 1983 usr/man/cat7/
 rwxrwxrwx1190/70      0 Sep 28 17:59 1983 usr/man/cat8/
 rwxrwxr-x1056/70      0 Nov 17 11:51 1983 usr/lib/
 rw-rw---- 12/70     93 Sep 28 13:22 1983 usr/lib/crontab
 rw-r--r--1086/70  18499 Nov 16 18:13 1983 usr/lib/whatis
 rwxrwx---1086/70 136312 Nov 16 17:57 1983 usr/lib/pcom
 rwxrwx---1086/70   2422 Nov 15 17:47 1983 usr/lib/lib.b
 rwxrwx---1086/70  31886 Nov 15 17:50 1983 usr/lib/libPW.a
 rwxrwx---1086/70   5016 Nov 15 17:51 1983 usr/lib/libtermlib.a
 rwxrwx---1086/70  40602 Nov 15 17:48 1983 usr/lib/libcurses.a
 rwxrwx--- 12/70      0 Nov 15 17:53 1983 usr/lib/tmac/
 r--r-----1024/70    450 Jun  7 18:31 1983 usr/lib/tmac/makefile
 r--r-----1024/70   7726 Jun  7 18:31 1983 usr/lib/tmac/tmac.a
 r--r--r--1119/70   4313 Sep  8 18:55 1983 usr/lib/tmac/tmac.an
 rw-r--r--1119/70     36 Sep  8 18:54 1983 usr/lib/tmac/.emacs_1119
 r--r-----1024/70   2226 Jun  7 18:31 1983 usr/lib/tmac/tmac.cp
 r--r-----1024/70    163 Jun  7 18:31 1983 usr/lib/tmac/tmac.m
 r--r-----1024/70   1349 Jun  7 18:31 1983 usr/lib/tmac/tmac.r
 r--r-----1024/70  23324 Jun  7 18:31 1983 usr/lib/tmac/tmac.s
 rw-r-----1086/70  12056 Nov 15 17:53 1983 usr/lib/tmac/tmac.e
 r--r-----1024/70   4398 Jun  7 18:31 1983 usr/lib/tmac/tmac.scover
 r--r-----1024/70    906 Jun  7 18:31 1983 usr/lib/tmac/tmac.sdisp
 r--r-----1024/70   1287 Jun  7 18:31 1983 usr/lib/tmac/tmac.skeep
 r--r-----1024/70   2201 Jun  7 18:31 1983 usr/lib/tmac/tmac.srefs
 r--r-----1024/70    133 Jun  7 18:31 1983 usr/lib/tmac/tmac.vcat
 r--r-----1024/70    776 Jun  7 18:31 1983 usr/lib/tmac/tmac.vgrind
 rwxrwx--- 12/70      0 Nov 16 18:40 1983 usr/lib/term/
 rwxrwx---1086/70   1715 Nov 16 18:40 1983 usr/lib/term/tab300
 rwxrwx---1086/70   1717 Nov 16 18:40 1983 usr/lib/term/tab300S
 rwxrwx---1086/70   1715 Nov 16 18:40 1983 usr/lib/term/tab300X
 rwxrwx---1086/70   1717 Nov 16 18:40 1983 usr/lib/term/tab300s
 rwxrwx---1086/70   1584 Nov 16 18:40 1983 usr/lib/term/tab37
 rwxrwx---1086/70   1739 Nov 16 18:40 1983 usr/lib/term/tab450-12-8
 rwxrwx---1086/70   1733 Nov 16 18:40 1983 usr/lib/term/tab450-12


                                 433010137-001                     Page 72 of 93


                                  GENIX SOURCE


 rwxrwx---1086/70   1721 Nov 16 18:40 1983 usr/lib/term/tab450
 rwxrwx---1086/70   1721 Nov 16 18:40 1983 usr/lib/term/tab450X
 rwxrwx---1086/70   1409 Nov 16 18:40 1983 usr/lib/term/tabtn300
 rwxrwx---1086/70   1722 Nov 16 18:40 1983 usr/lib/term/tabx1700
 rwxrwx---1086/70   1585 Nov 16 18:40 1983 usr/lib/term/tabdm
 rwxrwx---1086/70   1585 Nov 16 18:40 1983 usr/lib/term/tabh2
 rwxrwx---1086/70   1438 Nov 16 18:40 1983 usr/lib/term/tablpr
 rwxrwx---1086/70   1444 Nov 16 18:40 1983 usr/lib/term/tablpr2
 rwxrwx---1086/70  21180 Nov 16 17:57 1983 usr/lib/pmsg
 rwxrwx---1086/70   6672 Nov 16 18:10 1983 usr/lib/makekey
 rwxrwx---1086/70    706 Nov 16 18:49 1983 usr/lib/eign
 rwxrwx---1086/70   4656 Nov 15 17:48 1983 usr/lib/libjobs.a
 rwxrwx---1086/70   3498 Nov 15 17:49 1983 usr/lib/libln.a
 rwxrwx---1086/70   9412 Nov 16 18:13 1983 usr/lib/calendar
 rwxrwx---1086/70  15092 Nov 16 18:10 1983 usr/lib/atrun
 rwxrwx---1086/70   3498 Nov 15 17:49 1983 usr/lib/libl.a
                                 linked to usr/lib/libln.a
 rwxrwx---1086/70   5016 Nov 15 17:51 1983 usr/lib/libtermcap.a
                                 linked to usr/lib/libtermlib.a
 rwxrwx---1086/70   3488 Nov 16 18:49 1983 usr/lib/yaccpar
 rwsr-xr-x1086/70  16660 Nov 16 18:19 1983 usr/lib/ex3.6recover
 rwsr-xr-x1086/70  16248 Nov 16 18:20 1983 usr/lib/ex3.6preserve
 rwxrwx---1086/70  12376 Nov 16 18:17 1983 usr/lib/diff3
 rwxrwx---1009/70      0 Nov 16 18:20 1983 usr/lib/lex/
 rwxrwx---1086/70   4009 Nov 16 18:20 1983 usr/lib/lex/ncform
 rwxrwx---1086/70  14412 Nov 16 18:21 1983 usr/lib/lpf
 rwxrwx---1086/70   8292 Nov 16 18:21 1983 usr/lib/lpd
 rwxrwx---1086/70  10542 Nov 15 17:48 1983 usr/lib/libdbm.a
 rwxrwx---1086/70  10928 Nov 16 18:10 1983 usr/lib/getNAME
 rwxrwx---1086/70   5372 Nov 15 17:50 1983 usr/lib/libndir.a
 rwxrwx---1086/70  21564 Nov 16 18:16 1983 usr/lib/deroff
 rwxrwx---1086/70  15932 Nov 16 18:28 1983 usr/lib/spell
 rwxrwx---1086/70    647 Nov 16 18:10 1983 usr/lib/makewhatis
 rwxrwx---1086/70     20 Nov 17 11:51 1983 usr/lib/Mail.rc
 rwxrwx---1086/70  19220 Nov 15 17:49 1983 usr/lib/libm.a
 rwxrwx---1086/70  70060 Nov 16 18:15 1983 usr/lib/style1
 rwxrwx---1086/70    893 Nov 16 18:47 1983 usr/lib/Mail.help
 rwxrwx---1086/70    656 Nov 16 18:47 1983 usr/lib/Mail.help.~
 rwxrwx---1024/70      0 Jun 16 20:10 1983 usr/lib/macros/
 r--r-----1024/70  37824 May 26 16:54 1983 usr/lib/macros/mmn
 r--r-----1024/70  38397 May 26 16:55 1983 usr/lib/macros/mmt
 r--r-----1024/70    350 Jun  7 12:20 1983 usr/lib/macros/makefile
 rwxrwx---1024/70      0 Nov 16 18:43 1983 usr/lib/font/
 rwxrwx---1086/70    224 Nov 16 18:42 1983 usr/lib/font/ftB
 rwxrwx---1086/70    224 Nov 16 18:42 1983 usr/lib/font/ftBC
 rwxrwx---1086/70    224 Nov 16 18:42 1983 usr/lib/font/ftC
 rwxrwx---1086/70    224 Nov 16 18:42 1983 usr/lib/font/ftCE
 rwxrwx---1086/70    224 Nov 16 18:43 1983 usr/lib/font/ftH
 rwxrwx---1086/70    224 Nov 16 18:42 1983 usr/lib/font/ftCK
 rwxrwx---1086/70    224 Nov 16 18:42 1983 usr/lib/font/ftCS
 rwxrwx---1086/70    224 Nov 16 18:43 1983 usr/lib/font/ftCW
 rwxrwx---1086/70       224    Nov    16     18:43     1983     usr/lib/font/ftG
linked to usr/lib/font/ftH
 rwxrwx---1086/70    224 Nov 16 18:43 1983 usr/lib/font/ftGI
 rwxrwx---1086/70    224 Nov 16 18:43 1983 usr/lib/font/ftGM
 rwxrwx---1086/70    224 Nov 16 18:43 1983 usr/lib/font/ftGR
 rwxrwx---1086/70    224 Nov 16 18:43 1983 usr/lib/font/ftI
 rwxrwx---1086/70    224 Nov 16 18:43 1983 usr/lib/font/ftL


                                 433010137-001                     Page 73 of 93


                                  GENIX SOURCE


 rwxrwx---1086/70    224 Nov 16 18:43 1983 usr/lib/font/ftLI
 rwxrwx---1086/70    224 Nov 16 18:43 1983 usr/lib/font/ftPA
 rwxrwx---1086/70    224 Nov 16 18:43 1983 usr/lib/font/ftPB
 rwxrwx---1086/70    224 Nov 16 18:43 1983 usr/lib/font/ftPI
 rwxrwx---1086/70    224 Nov 16 18:43 1983 usr/lib/font/ftR
 rwxrwx---1086/70    224 Nov 16 18:43 1983 usr/lib/font/ftS
 rwxrwx---1086/70    224 Nov 16 18:43 1983 usr/lib/font/ftSB
 rwxrwx---1086/70    224 Nov 16 18:43 1983 usr/lib/font/ftSI
 rwxrwx---1086/70    224 Nov 16 18:43 1983 usr/lib/font/ftSM
 rwxrwx---1086/70    224 Nov 16 18:43 1983 usr/lib/font/ftUD
 rwxrwx---1086/70    224 Nov 16 18:43 1983 usr/lib/font/ftXM
 rwxrwx---1086/70    224 Nov 16 18:43 1983 usr/lib/font/ftHI
                                 linked to usr/lib/font/ftGI
 rwxrwx---1086/70    224 Nov 16 18:43 1983 usr/lib/font/ftHM
                                 linked to usr/lib/font/ftGM
 rwxrwx---1086/70    224 Nov 16 18:43 1983 usr/lib/font/ftFD
                                 linked to usr/lib/font/ftUD
 rwxrwx---1086/70    224 Nov 16 18:42 1983 usr/lib/font/ftCI
                                 linked to usr/lib/font/ftCK
 rwxrwx---1086/70  12066 Nov 15 17:49 1983 usr/lib/libmp.a
 rwxrwx---1086/70   1268 Nov 15 17:50 1983 usr/lib/libnsc.a
 rwxrwx---1086/70  59560 Nov 16 18:15 1983 usr/lib/style2
 rwxrwx---1086/70  26300 Nov 16 18:16 1983 usr/lib/style3
 rwxrwx---1086/70   8164 Nov 15 17:46 1983 usr/lib/libplot.a
 rwxrwx---1086/70   8270 Nov 16 18:16 1983 usr/lib/dict.d
 rwxrwx---1086/70  14219 Nov 16 18:16 1983 usr/lib/explain.d
 rwxrwx---1086/70  11508 Nov 16 18:16 1983 usr/lib/dprog
 rwxrwx---1086/70  11572 Nov 16 18:16 1983 usr/lib/diffh
 rwxrwx--- 12/70      0 Nov 16 18:44 1983 usr/lib/tabset/
 rw-rw-r--1086/70     93 Nov 16 18:43 1983 usr/lib/tabset/3101
 rw-rw-r--1086/70    105 Nov 16 18:43 1983 usr/lib/tabset/aa
 rw-rw-r--1086/70    164 Nov 16 18:43 1983 usr/lib/tabset/beehive
 rw-rw-r--1086/70     88 Nov 16 18:44 1983 usr/lib/tabset/diablo
 rw-rw-r--1086/70    135 Nov 16 18:44 1983 usr/lib/tabset/std
 rw-rw-r--1086/70     95 Nov 16 18:44 1983 usr/lib/tabset/stdcrt
 rw-rw-r--1086/70     57 Nov 16 18:44 1983 usr/lib/tabset/teleray
 rw-rw-r--1086/70    159 Nov 16 18:44 1983 usr/lib/tabset/vt100
 rw-rw-r--1086/70    164 Nov 16 18:44 1983 usr/lib/tabset/xerox1720
 rwxrwx---1086/70    161 Nov 16 18:30 1983 usr/lib/tabset/direct
 rw-rw-rw-1086/70   6324 Nov 16 18:19 1983 usr/lib/ex3.6strings
 rwxr-xr-x1134/70  24986 Oct 15 14:23 1983 usr/lib/libq.a
 rwxrwx---1086/70   8652 Nov 16 18:10 1983 usr/lib/units
 rw-r-----1086/70  24986 Nov 15 17:33 1983 usr/lib/libp.a
 rw-rw----1056/70  30304 Oct 18 17:06 1983 usr/lib/libsa.a
 rwxrwx---1086/70  10220 Nov 16 18:21 1983 usr/lib/slpd
 rwxr-xr-x1134/70  24986 Oct 15 14:23 1983 usr/lib/libqc.a
                                 linked to usr/lib/libq.a
 rwxrwx---1086/70      0 Nov 16 18:21 1983 usr/lib/lint/
 rwxrwx---1086/70 141935 Nov 16 18:21 1983 usr/lib/lint/lint1
 rwxrwx---1086/70 140296 Nov 16 18:21 1983 usr/lib/lint/lint2
 rwxrwx---1086/70   5698 Nov 16 18:21 1983 usr/lib/lint/llib-lc.ln
 rwxrwx---1086/70   2147 Nov 16 18:21 1983 usr/lib/lint/llib-port.ln
 rwxrwx---1086/70  10886 Nov 15 17:47 1983 usr/lib/lib300.a
 rwxrwx---1086/70  14468 Nov 16 18:21 1983 usr/lib/slpf
 rw-r-----1086/70  24986 Nov 15 17:33 1983 usr/lib/libpc.a
                                 linked to usr/lib/libp.a
 rwxrwx---1086/70  10966 Nov 15 17:47 1983 usr/lib/lib300s.a
 rwxrwx---1086/70  10170 Nov 15 17:47 1983 usr/lib/lib4014.a


                                 433010137-001                     Page 74 of 93


                                  GENIX SOURCE


 rwxrwx---1086/70  10864 Nov 15 17:47 1983 usr/lib/lib450.a
 rwxrwx---1086/70   8568 Nov 15 17:48 1983 usr/lib/libvt0.a
 rwxrwx---1024/70      0 Nov 15 17:53 1983 usr/lib/me/
 rwxrwx---1024/70      0 Nov 15 17:53 1983 usr/lib/me/src/
 r--r-----1086/70    671 Jun  7 17:39 1983 usr/lib/me/src/makefile
 r--r-----1086/70    699 Jun  7 11:51 1983 usr/lib/me/src/acm.me
 r--r-----1086/70   1717 Jun  7 11:51 1983 usr/lib/me/src/chars.me
 r--r-----1086/70    364 Jun  7 11:51 1983 usr/lib/me/src/deltext.me
 r--r-----1086/70   1171 Jun  7 11:51 1983 usr/lib/me/src/eqn.me
 r--r-----1086/70   1047 Jun  7 11:51 1983 usr/lib/me/src/float.me
 r--r-----1086/70   1380 Jun  7 11:51 1983 usr/lib/me/src/footnote.me
 r--r-----1086/70   1027 Jun  7 11:51 1983 usr/lib/me/src/index.me
 r--r-----1086/70    451 Jun  7 11:51 1983 usr/lib/me/src/local.me
 r--r-----1086/70    272 Jun  7 11:51 1983 usr/lib/me/src/null.me
 r--r-----1086/70   1781 Jun  7 11:51 1983 usr/lib/me/src/sh.me
 r--r-----1086/70   1101 Jun  7 11:51 1983 usr/lib/me/src/tbl.me
 r--r-----1086/70    593 Jun  7 11:51 1983 usr/lib/me/src/thesis.me
 r--r-----1086/70  19203 Jun  7 11:51 1983 usr/lib/me/src/tmac.e
 r-xr-x---1086/70    714 Jun  7 11:51 1983 usr/lib/me/src/install.csh
 r--r-----1086/70    174 Jun  7 11:51 1983 usr/lib/me/src/test.accent
 r--r-----1086/70    415 Jun  7 11:51 1983 usr/lib/me/src/test.umlaut
 r--r-----1086/70   4607 Jun  7 11:51 1983 usr/lib/me/src/revisions
 rw-r-----1086/70    738 Nov 15 17:51 1983 usr/lib/me/acm.me
 rw-r-----1086/70   1546 Nov 15 17:51 1983 usr/lib/me/chars.me
 rw-r-----1086/70    344 Nov 15 17:51 1983 usr/lib/me/deltext.me
 rw-r-----1086/70    976 Nov 15 17:51 1983 usr/lib/me/eqn.me
 rw-r-----1086/70    831 Nov 15 17:51 1983 usr/lib/me/float.me
 rw-r-----1086/70   1132 Nov 15 17:52 1983 usr/lib/me/footnote.me
 rw-r-----1086/70    855 Nov 15 17:52 1983 usr/lib/me/index.me
 rw-r-----1086/70    513 Nov 15 17:52 1983 usr/lib/me/local.me
 rw-r-----1086/70    185 Nov 15 17:52 1983 usr/lib/me/null.me
 rw-r-----1086/70   1558 Nov 15 17:52 1983 usr/lib/me/sh.me
 rw-r-----1086/70    976 Nov 15 17:52 1983 usr/lib/me/tbl.me
 rw-r-----1086/70    528 Nov 15 17:52 1983 usr/lib/me/thesis.me
 r--r-----1086/70   4607 Nov 15 17:53 1983 usr/lib/me/revisions
 rwxrwx---1119/70      0 Nov 16 18:48 1983 usr/lib/uucp/
 rwxrwx---1086/70  26468 Nov 16 18:48 1983 usr/lib/uucp/uuclean
 rwxrwx---1086/70  86527 Nov 16 18:48 1983 usr/lib/uucp/uucico
 rwxrwx---1086/70  35960 Nov 16 18:48 1983 usr/lib/uucp/uusub
 rwxrwx---1086/70  46389 Nov 16 18:48 1983 usr/lib/uucp/uuxqt
 rw-rw----1119/70    151 Aug 22 15:59 1983 usr/lib/uucp/L.sys
 rw-rw----1119/70     50 Aug 22 15:51 1983 usr/lib/uucp/L-devices
 rw-rw----1119/70     13 Aug 22 15:58 1983 usr/lib/uucp/L-dialcodes
 rw-rw----1119/70     12 Aug 22 15:54 1983 usr/lib/uucp/USERFILE
 rw-rw----1119/70    299 Aug 22 15:57 1983 usr/lib/uucp/README
 rw-rw----1119/70      0 Aug 25 13:19 1983 usr/lib/uucp/SEQF
 rw-r--r--1119/70    517 Aug 25 12:53 1983 usr/lib/aliases
 rwxrwx---1086/70      0 Nov 16 18:49 1983 usr/lib/cref/
 rwxrwx---1086/70   9064 Nov 16 18:49 1983 usr/lib/cref/crpost
 rwxrwx---1086/70   9268 Nov 16 18:49 1983 usr/lib/cref/upost
 rwxrwx---1086/70    768 Nov 16 18:49 1983 usr/lib/cref/atab
 rwxrwx---1086/70    768 Nov 16 18:49 1983 usr/lib/cref/ctab
 rwxrwx---1086/70    768 Nov 16 18:49 1983 usr/lib/cref/etab
 rwxrwx---1086/70   5024 Nov 16 18:49 1983 usr/lib/cref/aign
 rwxrwx---1086/70   5024 Nov 16 18:49 1983 usr/lib/cref/cign
 rwxrwx---1086/70    706 Nov 16 18:49 1983 usr/lib/cref/eign
                                 linked to usr/lib/eign
 rwxrwx---1056/70      0 Nov 16 18:52 1983 usr/nsc/


                                 433010137-001                     Page 75 of 93


                                  GENIX SOURCE


 rwxrwx---1086/70  19404 Nov 16 18:51 1983 usr/nsc/sash
 rwxrwx---1086/70   7924 Nov 16 18:10 1983 usr/nsc/burn
 rwxrwx---1086/70  10356 Nov 16 18:11 1983 usr/nsc/tcerr
 rwxrwxrwx1004/70     47 Aug 13 12:24 1982 usr/nsc/dir
 rwxrwx---1086/70  12108 Nov 16 18:11 1983 usr/nsc/csym
 rwxrwx---1086/70  16228 Nov 16 18:11 1983 usr/nsc/remind
 rwxrwx--- 12/70      0 Jun 16 14:49 1983 usr/nsc/lib/
 rwxrwxr-x1119/70      0 Nov 16 18:28 1983 usr/nsc/lib/help/
 rwxrwx---1086/70   4144 Nov 16 18:27 1983 usr/nsc/lib/help/ad
 rwxrwx---1086/70   1072 Nov 16 18:27 1983 usr/nsc/lib/help/bd
 rwxrwx---1086/70    648 Nov 16 18:27 1983 usr/nsc/lib/help/cb
 rwxrwx---1086/70   2171 Nov 16 18:27 1983 usr/nsc/lib/help/cm
 rwxrwx---1086/70   5431 Nov 16 18:27 1983 usr/nsc/lib/help/cmds
 rwxrwx---1086/70   3663 Nov 16 18:27 1983 usr/nsc/lib/help/co
 rwxrwx---1086/70   3333 Nov 16 18:27 1983 usr/nsc/lib/help/de
 rwxrwx---1086/70    150 Nov 16 18:27 1983 usr/nsc/lib/help/default
 rwxrwx---1086/70   2030 Nov 16 18:27 1983 usr/nsc/lib/help/ge
 rwxrwx---1086/70    309 Nov 16 18:28 1983 usr/nsc/lib/help/he
 rwxrwx---1086/70    490 Nov 16 18:28 1983 usr/nsc/lib/help/prs
 rwxrwx---1086/70   2261 Nov 16 18:28 1983 usr/nsc/lib/help/rc
 rwxrwx---1086/70    559 Nov 16 18:28 1983 usr/nsc/lib/help/un
 rwxrwx---1086/70   2070 Nov 16 18:28 1983 usr/nsc/lib/help/ut
 rwxrwx---1086/70   2417 Nov 16 18:28 1983 usr/nsc/lib/help/vc
 rwxrwx---1086/70  91535 Nov 16 18:14 1983 usr/nsc/ddt
 rwxrwx---1086/70  15876 Nov 16 18:11 1983 usr/nsc/cu16
 rwxrwx---1086/70  15092 Nov 16 18:51 1983 usr/nsc/mnm
 rwxrwx---1086/70   9904 Nov 16 18:11 1983 usr/nsc/dcerr
 rwxrwx---1086/70  58602 Nov 16 18:24 1983 usr/nsc/admin
 rwxrwx---1086/70  28849 Nov 16 18:24 1983 usr/nsc/bdiff
 rwxrwx---1077/70  10408 Feb 24 15:27 1983 usr/nsc/nleave
 rwxrwx---1086/70  48892 Nov 16 18:24 1983 usr/nsc/comb
 rwxrwx---1086/70  69286 Nov 16 18:24 1983 usr/nsc/delta
 rwxrwx---1086/70  65745 Nov 16 18:25 1983 usr/nsc/get
 rwxrwx---1086/70  20965 Nov 16 18:25 1983 usr/nsc/sccshelp
 rwxrwx---1086/70  57576 Nov 16 18:25 1983 usr/nsc/prs
 rwxrwx---1086/70  31889 Nov 16 18:25 1983 usr/nsc/prt
 rwxrwx---1086/70  58712 Nov 16 18:26 1983 usr/nsc/rmchg
 rwxrwx---1086/70  36248 Nov 16 18:26 1983 usr/nsc/sccs
 rwxrwx---1086/70  47464 Nov 16 18:26 1983 usr/nsc/unget
 rwxrwx---1086/70  23431 Nov 16 18:26 1983 usr/nsc/val
 rwxrwx---1086/70  12700 Nov 16 18:11 1983 usr/nsc/monfix
 rwxrwx---1086/70  17259 Nov 16 18:26 1983 usr/nsc/what
 rwxrwx---1086/70  58712 Nov 16 18:26 1983 usr/nsc/cdc
                                 linked to usr/nsc/rmchg
 rwxrwx---1086/70  58712 Nov 16 18:26 1983 usr/nsc/rmdel
                                 linked to usr/nsc/rmchg
 rwxrwx---1086/70  47464 Nov 16 18:26 1983 usr/nsc/sact
                                 linked to usr/nsc/unget
 rwxrwx---1086/70   1303 Nov 16 18:27 1983 usr/nsc/sccsdiff
 rwxrwx---1086/70  11696 Nov 16 18:11 1983 usr/nsc/nburn
 rwxrwx---1086/70  19024 Nov 16 18:11 1983 usr/nsc/su1
 rwxrwx---1086/70   9848 Nov 16 18:51 1983 usr/nsc/getscreen
 rwxrwx---1086/70  10932 Nov 16 18:51 1983 usr/nsc/putscreen
 rwxrwx---1086/70  13192 Nov 16 18:11 1983 usr/nsc/mmerr
 rwxrwx---1086/70  11020 Nov 16 18:11 1983 usr/nsc/uvers
 rwxrwx---1086/70  28932 Nov 16 18:17 1983 usr/nsc/dpy
 rwxrwx---1086/70  13980 Nov 16 18:51 1983 usr/nsc/ptycon
 rwxrwx---1086/70  12536 Nov 16 18:51 1983 usr/nsc/ff


                                 433010137-001                     Page 76 of 93


                                  GENIX SOURCE


 rwxrwx---1086/70    479 Nov 16 18:51 1983 usr/nsc/cleanup
 rwxrwx---1086/70  15532 Nov 16 18:11 1983 usr/nsc/stat
 rwxrwx---1086/70  13000 Nov 16 18:11 1983 usr/nsc/uid
 rwxrwx---1086/70 127743 Nov 16 18:52 1983 usr/nsc/testsys
 rwxrwx---1086/70  13292 Nov 16 18:52 1983 usr/nsc/vc
 rwxr-xr-x1056/70  75672 Jul 10 17:56 1983 usr/nsc/cshrec
 rwxrwx---1056/70      0 Jun 16 16:52 1983 usr/pub/
 rw-r-----1056/70   2114 Jul 27 02:50 1982 usr/pub/ascii
 rw-rw----1024/70   2969 Jun 16 16:52 1983 usr/pub/eqnchar
 rw-rw----1024/70    475 Jun 16 16:52 1983 usr/pub/greek
 rwxrwx---1056/70      0 Nov 17 11:51 1983 usr/ucb/
 rwxrwx---1086/70  22216 Nov 16 18:08 1983 usr/ucb/more
 rwxrwx---1086/70  22216 Nov 16 18:08 1983 usr/ucb/page
                                 linked to usr/ucb/more
 rwxrwx---1086/70   6188 Nov 16 18:07 1983 usr/ucb/clear
 rwxrwx---1086/70  22132 Nov 16 18:22 1983 usr/ucb/ls
 rwxrwx---1086/70  11028 Nov 16 18:08 1983 usr/ucb/head
 rwxrwx---1086/70   7916 Nov 16 18:08 1983 usr/ucb/printenv
 rwxrwx---1086/70   5916 Nov 16 18:08 1983 usr/ucb/reset
 rwxrwx---1086/70  13576 Nov 16 18:43 1983 usr/ucb/tset
 rwxrwx---1086/70  11332 Nov 16 18:10 1983 usr/ucb/whoami
 rwxrwx---1086/70   7916 Nov 16 18:10 1983 usr/ucb/yes
 rwxrwx---1086/70  22132 Nov 16 18:22 1983 usr/ucb/l
                                 linked to usr/ucb/ls
 rwxrwx---1086/70  14660 Nov 16 18:09 1983 usr/ucb/ul
 rwxrwx---1086/70  11612 Nov 16 18:07 1983 usr/ucb/chfn
 rwxrwx---1086/70  11984 Nov 16 18:07 1983 usr/ucb/colcrt
 rwxrwx---1086/70   4752 Nov 16 18:07 1983 usr/ucb/colrm
 rwxrwx---1086/70  10932 Nov 16 18:07 1983 usr/ucb/expand
 rwxrwx---1086/70  10992 Nov 16 18:09 1983 usr/ucb/unexpand
 rwxrwx---1086/70  10940 Nov 16 18:07 1983 usr/ucb/fold
 rwxr-xr-x1086/70 119564 Nov 16 18:19 1983 usr/ucb/vi
 rwxrwx---1086/70  13320 Nov 16 18:09 1983 usr/ucb/strings
 rwxrwx---1086/70  11856 Nov 16 18:10 1983 usr/ucb/whereis
 rwxrwx---1086/70  14260 Nov 16 18:13 1983 usr/ucb/compact
 rwxrwx---1086/70  13192 Nov 16 18:14 1983 usr/ucb/uncompact
 rwxr-xr-x1086/70    168 Nov 16 18:14 1983 usr/ucb/ccat
 rwxrwx---1086/70   9096 Nov 16 18:08 1983 usr/ucb/lock
 rwxrwx---1086/70  11964 Nov 16 18:10 1983 usr/ucb/what
 rwxrwx---1086/70  13380 Nov 16 18:08 1983 usr/ucb/num
 rwxrwx---1086/70  21368 Nov 16 18:07 1983 usr/ucb/finger
 rwxrwx---1086/70  20616 Nov 16 18:07 1983 usr/ucb/checknr
 rwxrwx---1086/70  11612 Nov 16 18:07 1983 usr/ucb/chsh
 rwxrwx---1086/70  17156 Nov 16 18:07 1983 usr/ucb/ctags
 rwxrwx---1086/70  12876 Nov 16 18:08 1983 usr/ucb/leave
 rwxrwx---1086/70  13276 Nov 16 18:08 1983 usr/ucb/mkstr
 rwxrwx---1086/70    131 Nov 16 18:09 1983 usr/ucb/see
 rwxrwx---1086/70  10952 Nov 16 18:09 1983 usr/ucb/soelim
 rwxrwx---1086/70    778 Nov 16 18:10 1983 usr/ucb/which
 rwxrwx---1086/70  21368 Nov 16 18:07 1983 usr/ucb/f
                                 linked to usr/ucb/finger
 rwxrwx---1086/70  64024 Nov 16 18:47 1983 usr/ucb/Mail
 rwxrwx---1086/70  13416 Nov 16 18:47 1983 usr/ucb/fmt
 rwxrwx---1086/70  64024 Nov 16 18:47 1983 usr/ucb/mail
                                 linked to usr/ucb/Mail
 rwxrwx---1086/70  12164 Nov 16 18:22 1983 usr/ucb/lpq
 rwxrwx---1086/70   9064 Nov 16 18:08 1983 usr/ucb/gets
 rwxrwx---1086/70  14712 Nov 16 18:08 1983 usr/ucb/last


                                 433010137-001                     Page 77 of 93


                                  GENIX SOURCE


 rwxrwx---1086/70   1084 Nov 16 18:08 1983 usr/ucb/help
 rwxrwx---1086/70  14468 Nov 16 18:08 1983 usr/ucb/lastcomm
 rwxrwx---1086/70    127 Nov 16 18:08 1983 usr/ucb/print
 rwxrwx---1086/70   7936 Nov 16 18:09 1983 usr/ucb/rewind
 rwxrwx---1086/70  10112 Nov 16 18:20 1983 usr/ucb/grep
 rwxrwx---1086/70  13800 Nov 16 18:22 1983 usr/ucb/lpr
 rwxrwx---1086/70  12148 Nov 16 18:22 1983 usr/ucb/lprm
 rwxr-xr-x1086/70 119564 Nov 16 18:19 1983 usr/ucb/edit
                                 linked to usr/ucb/vi
 rwxrwx---1086/70  10240 Nov 16 18:09 1983 usr/ucb/users
 rwxrwx---1086/70  10240 Nov 16 18:09 1983 usr/ucb/u
                                 linked to usr/ucb/users
 rwxrwx---1086/70  12096 Nov 16 18:09 1983 usr/ucb/vmstat
 rwxrwx---1086/70  16412 Nov 16 18:09 1983 usr/ucb/w
 rwxrwx---1086/70  16412 Nov 16 18:09 1983 usr/ucb/uptime
                                 linked to usr/ucb/w
 rwxrwx---1086/70    149 Nov 16 18:07 1983 usr/ucb/apropos
 rwxrwx---1086/70  11240 Nov 16 18:10 1983 usr/ucb/whatis
 rwxrwx---1086/70  13404 Nov 16 18:08 1983 usr/ucb/prmail
 rwxrwx---1086/70  18044 Nov 16 18:09 1983 usr/ucb/uusend
 rwxrwx---1086/70  13936 Nov 16 18:08 1983 usr/ucb/from
 rwxr-xr-x1086/70 119564 Nov 16 18:19 1983 usr/ucb/e
                                 linked to usr/ucb/vi
 rwxr-xr-x1086/70 119564 Nov 16 18:19 1983 usr/ucb/ex
                                 linked to usr/ucb/vi
 rwxr-xr-x1086/70 119564 Nov 16 18:19 1983 usr/ucb/view
                                 linked to usr/ucb/vi
 rwxrwx---1086/70  16352 Nov 16 18:23 1983 usr/ucb/man
 rwxrwx---1086/70  26700 Nov 16 18:23 1983 usr/ucb/msgs
 rwxrwx---1086/70  16840 Nov 16 18:09 1983 usr/ucb/uudecode
 rwxrwx---1086/70  10936 Nov 16 18:09 1983 usr/ucb/uuencode
 rwxrwx---1086/70    222 Nov 16 18:09 1983 usr/ucb/trman
 rwxrwx---1086/70  26840 Nov 17 11:51 1983 usr/ucb/newaliases
 rwxrwx---1009/70      0 Nov 16 18:49 1983 usr/bin/
 rwxrwx---1086/70  11400 Nov 16 18:05 1983 usr/bin/fgrep
 rwxrwx---1086/70   9036 Nov 16 18:06 1983 usr/bin/sleep
 rwxrwx---1086/70  11068 Nov 16 18:06 1983 usr/bin/tail
 rwxrwx---1086/70  10260 Nov 16 18:05 1983 usr/bin/crypt
 rwxrwx---1086/70   8992 Nov 16 18:06 1983 usr/bin/touch
 rwxrwx---1086/70  10160 Nov 16 18:05 1983 usr/bin/comm
 rwxrwx---1086/70   4728 Nov 16 18:06 1983 usr/bin/tr
 rwxrwx---1086/70  12364 Nov 16 18:07 1983 usr/bin/uniq
 rwxrwx---1086/70  27624 Nov 16 18:14 1983 usr/bin/dc
 rwxrwx---1086/70   9228 Nov 16 18:05 1983 usr/bin/cal
 rwxrwx---1086/70  11148 Nov 16 18:05 1983 usr/bin/col
 rwxrwx---1086/70  10364 Nov 16 18:05 1983 usr/bin/look
 rwxrwx---1086/70  57128 Nov 16 18:31 1983 usr/bin/troff
 rwxrwx---1086/70  52784 Nov 16 18:31 1983 usr/bin/nroff
 rwxrwx---1086/70   9048 Nov 16 18:06 1983 usr/bin/rev
 rwxrwx---1086/70   9124 Nov 16 18:06 1983 usr/bin/sum
 rwxrwx---1086/70  10136 Nov 16 18:06 1983 usr/bin/split
 rwxrwx---1086/70   4696 Nov 16 18:05 1983 usr/bin/basename
 rwxrwx---1086/70   9084 Nov 16 18:06 1983 usr/bin/mesg
 rwxrwx---1086/70  13300 Nov 16 18:05 1983 usr/bin/file
 rwxrwx---1086/70  18904 Nov 16 18:05 1983 usr/bin/find
 rwxrwx---1086/70  10304 Nov 16 18:07 1983 usr/bin/wc
 rwxrwx---1086/70  11292 Nov 16 17:57 1983 usr/bin/pc
 rwxrwx---1086/70  13948 Nov 16 18:06 1983 usr/bin/ptx


                                 433010137-001                     Page 78 of 93


                                  GENIX SOURCE


 rwxrwx---1086/70  12648 Nov 16 18:07 1983 usr/bin/tsort
 rwxrwx---1086/70   9028 Nov 16 18:07 1983 usr/bin/tty
 rwxrwx---1086/70  15568 Nov 16 18:05 1983 usr/bin/at
 rwxrwx---1086/70  18712 Nov 16 18:05 1983 usr/bin/bc
 rwxrwx---1086/70  11256 Nov 16 18:05 1983 usr/bin/cb
 rwxrwx---1086/70    470 Nov 16 18:16 1983 usr/bin/diction
 rwxrwx---1086/70  10388 Nov 16 18:05 1983 usr/bin/checkeq
 rwxrwx---1086/70  18696 Nov 16 18:05 1983 usr/bin/cu
 rwxrwx---1086/70  20288 Nov 16 18:06 1983 usr/bin/newgrp
 rwxrwx---1086/70  14160 Nov 16 18:05 1983 usr/bin/egrep
 rwxrwx---1086/70  15604 Nov 16 18:06 1983 usr/bin/sort
 rwxrwx---1086/70   2211 Nov 16 18:06 1983 usr/bin/minstall
 rwxrwx---1086/70  36480 Nov 16 18:18 1983 usr/bin/eqn
 rwxrwx---1086/70  18284 Nov 16 18:22 1983 usr/bin/m4
 rwxrwx---1086/70  50598 Nov 16 18:48 1983 usr/bin/uucp
 rwxrwx---1086/70  37692 Nov 16 18:20 1983 usr/bin/lex
 rwxrwx---1086/70  37508 Nov 16 18:30 1983 usr/bin/tbl
 rwxrwx---1086/70  15652 Nov 16 18:06 1983 usr/bin/prof
 rwxrwx---1086/70  12784 Nov 16 18:06 1983 usr/bin/ranlib
 rwxrwx---1086/70   5904 Nov 16 18:06 1983 usr/bin/tabs
 rwxrwx---1086/70    848 Nov 16 18:23 1983 usr/bin/man
 rwxrwx---1086/70  33616 Nov 16 18:23 1983 usr/bin/neqn
 rwxrwx---1086/70  21564 Nov 16 18:16 1983 usr/bin/deroff
                                 linked to usr/lib/deroff
 rwxrwx---1086/70  10244 Nov 16 18:29 1983 usr/bin/spellin
 rwxrwx---1086/70  10268 Nov 16 18:29 1983 usr/bin/spellout
 rwxrwx---1086/70    665 Nov 16 18:29 1983 usr/bin/spell
 rwxrwx---1086/70  30719 Nov 16 18:48 1983 usr/bin/uulog
 rwxrwx---1086/70  22524 Nov 16 18:48 1983 usr/bin/enroll
 rwxrwx---1086/70  33572 Nov 16 18:49 1983 usr/bin/yacc
 rwxrwx---1086/70  28380 Nov 16 18:48 1983 usr/bin/xsend
 rwxrwx---1086/70  11600 Nov 16 18:07 1983 usr/bin/units
 rwxrwx---1086/70  17148 Nov 16 18:48 1983 usr/bin/uuname
 rwxrwx---1086/70  28188 Nov 16 18:49 1983 usr/bin/xget
 rwxrwx---1086/70    625 Nov 16 18:13 1983 usr/bin/calendar
 rwxrwx---1086/70    327 Nov 16 18:16 1983 usr/bin/explain
 rwxrwx---1086/70    613 Nov 16 18:15 1983 usr/bin/style
 rwxrwx---1086/70    388 Nov 16 18:17 1983 usr/bin/diff3
 rwxrwx---1086/70    627 Nov 16 18:21 1983 usr/bin/lint
 rwxrwx---1086/70  39517 Nov 16 18:48 1983 usr/bin/uustat
 rwxrwx---1086/70  48599 Nov 16 18:48 1983 usr/bin/uux
 rwxrwx---1086/70  16192 Nov 16 18:49 1983 usr/bin/cref
 rwxrwx---1086/70   2211 Sep 28 14:58 1983 usr/bin/install
 rwxrwx---1056/70      0 Nov 16 18:29 1983 usr/dict/
 rw-r-----1056/70 201032 Oct  9 14:00 1982 usr/dict/words
 rwxrwx---1086/70  50000 Nov 16 18:29 1983 usr/dict/hlista
 rwxrwx---1086/70  50000 Nov 16 18:29 1983 usr/dict/hlistb
 rwxrwx---1086/70  50000 Nov 16 18:29 1983 usr/dict/hstop
 rwxrwxrwx1072/70      0 Jan 31 11:02 1983 usr/dict/spellhist
 rwxrwxrwx1009/70      0 Sep 21 11:02 1983 usr/tmp/
 rw-r--r--1190/70      0 Sep 21 11:02 1983 usr/tmp/.hushlogin
 rwxrwxrwx1009/70      0 Oct 19 14:59 1982 usr/msgs/
 rwxrwxr-x1056/70      0 Mar 17 14:04 1983 usr/adm/
 rw-r--r--1056/70      0 Nov 29 22:10 1982 usr/adm/lastlog
 rw-r--r--1056/70      0 Nov 29 22:32 1982 usr/adm/wtmp
 rw-r--r--1007/70      0 Mar 17 14:04 1983 usr/adm/msgbuf
 rwxrwxrwx1009/70      0 Sep 21 11:02 1983 usr/spool/
 rwxr-xr-x1009/70      0 Feb 24 01:55 1983 usr/spool/at/


                                 433010137-001                     Page 79 of 93


                                  GENIX SOURCE


 rwxr-xr-x 12/70      0 Feb 24 01:55 1983 usr/spool/at/past/
 rwxr-x---1009/70      0 Dec 27 09:58 1982 usr/spool/lpd/
 rwxrwxrwx1077/70      0 May 12 17:14 1983 usr/spool/mail/
 rwxrwx---1077/70      0 Nov 16 18:49 1983 usr/spool/secretmail/
 rwxrwx---1086/70     31 Nov 16 18:49 1983 usr/spool/secretmail/notice
 rwxrwx---1119/70      0 May  6 10:46 1983 usr/spool/uucp/
 rwxrwx---1119/70      0 May  6 10:46 1983 usr/spool/uucp/.XQTDIR/
 rwxrwx---1119/70      0 Aug  2 10:23 1983 usr/spool/uucppublic/
 r--r--r--1119/70      0 Aug  2 10:23 1983 usr/spool/uucppublic/.hushlogin
 rwxrwxrwx1190/70      0 Sep 21 11:02 1983 usr/spool/slp/
 rwxrwx---1004/70      0 Jan  4 11:44 1983 usr/preserve/

















































                                 433010137-001                     Page 80 of 93


                                  GENIX SOURCE


_T_a_p_e _3:  _G_E_N_I_X_1._0_A_I_I_I

 rwxr-xr-x1188/410      0 Dec  2 18:36 1983 man/
 rwxr-xr-x1188/410      0 Nov 28 15:31 1983 man/man1/
 rw-r--r--1188/410     16 Oct 26 11:09 1983 man/man1/diffh.1
 rw-r--r--1188/410   3888 Sep 28 16:30 1983 man/man1/cu.1c
 rw-r--r--1188/410   5575 Nov  2 16:05 1983 man/man1/awk.1
 rw-r--r--1188/410   1755 Sep 28 16:30 1983 man/man1/last.1
 rw-r--r--1188/410    459 Sep 28 16:30 1983 man/man1/newaliases.1
 rw-r--r--1188/410   1776 Nov  2 16:05 1983 man/man1/lpr.1
 rw-r--r--1188/410     17 Oct 26 11:09 1983 man/man1/uuname.1c
 rw-r--r--1188/410    819 Sep 28 16:30 1983 man/man1/cp.1
 rw-r--r--1188/410   3289 Sep 28 16:31 1983 man/man1/uustat.1c
 rw-r--r--1188/410   3077 Sep 28 16:31 1983 man/man1/ar.1
 rw-r--r--1188/410   3494 Sep 28 16:31 1983 man/man1/cu16.1
 rw-r--r--1188/410   1049 Sep 28 16:31 1983 man/man1/date.1
 rw-r--r--1188/410   8547 Sep 28 16:31 1983 man/man1/ps.1
 rw-r--r--1188/410   8022 Sep 28 16:31 1983 man/man1/stty.1
 rw-r--r--1188/410   3029 Sep 28 16:31 1983 man/man1/as.1
 rw-r--r--1188/410    832 Sep 28 16:31 1983 man/man1/basename.1
 rw-r--r--1188/410    643 Sep 28 16:31 1983 man/man1/cal.1
 rw-r--r--1188/410   1650 Sep 28 16:31 1983 man/man1/cat.1
 rw-r--r--1188/410    759 Sep 28 16:31 1983 man/man1/cd.1
 rw-r--r--1188/410   2287 Sep 28 16:31 1983 man/man1/checknr.1
 rw-r--r--1188/410   1626 Sep 28 16:31 1983 man/man1/chfn.1
 rw-r--r--1188/410   2068 Sep 28 16:31 1983 man/man1/chmod.1
 rw-r--r--1188/410    713 Sep 28 16:31 1983 man/man1/chsh.1
 rw-r--r--1188/410    410 Sep 28 16:31 1983 man/man1/clear.1
 rw-r--r--1188/410    860 Sep 28 16:31 1983 man/man1/cmp.1
 rw-r--r--1188/410   2139 Sep 28 16:31 1983 man/man1/col.1
 rw-r--r--1188/410   1727 Sep 28 16:31 1983 man/man1/colcrt.1
 rw-r--r--1188/410    644 Sep 28 16:31 1983 man/man1/colrm.1
 rw-r--r--1188/410   2440 Sep 28 16:34 1983 man/man1/w.1
 rw-r--r--1188/410    762 Sep 28 16:31 1983 man/man1/comm.1
 rw-r--r--1188/410   2174 Sep 28 16:31 1983 man/man1/compact.1
 rw-r--r--1188/410  21614 Sep 28 16:31 1983 man/man1/ddt.1
 rw-r--r--1188/410   2427 Sep 28 16:31 1983 man/man1/crypt.1
 rw-r--r--1188/410  60709 Sep 28 16:32 1983 man/man1/csh.1
 rw-r--r--1188/410   2887 Nov  2 16:07 1983 man/man1/ctags.1
 rw-r--r--1188/410   4321 Sep 28 16:32 1983 man/man1/dc.1
 rw-r--r--1188/410   3347 Sep 28 16:32 1983 man/man1/dd.1
 rw-r--r--1188/410   1019 Sep 28 16:32 1983 man/man1/df.1
 rw-r--r--1188/410   5161 Sep 28 16:32 1983 man/man1/diff.1
 rw-r--r--1188/410     17 Oct 26 11:09 1983 man/man1/nroff.1
 rw-r--r--1188/410    868 Sep 28 16:32 1983 man/man1/du.1
 rw-r--r--1188/410    528 Sep 28 16:32 1983 man/man1/echo.1
 rw-r--r--1188/410  18032 Sep 28 16:32 1983 man/man1/ed.1
 rw-r--r--1188/410   3194 Sep 28 16:32 1983 man/man1/ex.1
 rw-r--r--1188/410   1213 Sep 28 16:32 1983 man/man1/expand.1
 rw-r--r--1188/410     16 Oct 26 11:09 1983 man/man1/false.1
 rw-r--r--1188/410     16 Oct 26 11:09 1983 man/man1/fgrep.1
 rw-r--r--1188/410    509 Sep 28 16:32 1983 man/man1/file.1
 rw-r--r--1188/410   3077 Sep 28 16:32 1983 man/man1/find.1
 rw-r--r--1188/410   1923 Sep 28 16:32 1983 man/man1/finger.1
 rw-r--r--1188/410    658 Sep 28 16:32 1983 man/man1/fold.1
 rw-r--r--1188/410   1984 Sep 28 16:32 1983 man/man1/at.1
 rw-r--r--1188/410   3980 Nov  2 16:52 1983 man/man1/grep.1
 rw-r--r--1188/410    372 Sep 28 16:32 1983 man/man1/head.1


                                 433010137-001                     Page 81 of 93


                                  GENIX SOURCE


 rw-r--r--1188/410   1414 Sep 28 16:32 1983 man/man1/intro.1
 rw-r--r--1188/410   1839 Sep 28 16:32 1983 man/man1/kill.1
 rw-r--r--1188/410   1089 Sep 28 16:32 1983 man/man1/leave.1
 rw-r--r--1188/410   1175 Sep 28 16:32 1983 man/man1/ln.1
 rw-r--r--1188/410    501 Sep 28 16:32 1983 man/man1/lock.1
 rw-r--r--1188/410   2405 Sep 28 16:32 1983 man/man1/login.1
 rw-r--r--1188/410    703 Sep 28 16:32 1983 man/man1/look.1
 rw-r--r--1188/410   5845 Sep 28 16:32 1983 man/man1/ls.1
 rw-r--r--1188/410   5188 Sep 28 16:32 1983 man/man1/make.1
 rw-r--r--1188/410    566 Sep 28 16:32 1983 man/man1/mesg.1
 rw-r--r--1188/410    587 Sep 28 16:32 1983 man/man1/mkdir.1
 rw-r--r--1188/410   2300 Sep 28 16:32 1983 man/man1/mkstr.1
 rw-r--r--1188/410   5298 Sep 28 16:32 1983 man/man1/ld.1
 rw-r--r--1188/410   8197 Sep 28 16:32 1983 man/man1/more.1
 rw-r--r--1188/410   1605 Sep 28 16:33 1983 man/man1/mv.1
 rw-r--r--1188/410   3576 Sep 28 16:33 1983 man/man1/dpy.1
 rw-r--r--1188/410    102 Oct 26 11:09 1983 man/man1/edit.1
 rw-r--r--1188/410    527 Sep 28 16:33 1983 man/man1/num.1
 rw-r--r--1188/410   1746 Sep 28 16:33 1983 man/man1/monfix.1
 rw-r--r--1188/410   1576 Sep 28 16:33 1983 man/man1/od.1
 rw-r--r--1188/410   1601 Sep 28 16:33 1983 man/man1/nburn.1
 rw-r--r--1188/410     16 Oct 26 11:09 1983 man/man1/page.1
 rw-r--r--1188/410   1060 Sep 28 16:33 1983 man/man1/passwd.1
 rw-r--r--1188/410   1588 Sep 28 16:33 1983 man/man1/pr.1
 rw-r--r--1188/410    336 Sep 28 16:33 1983 man/man1/print.1
 rw-r--r--1188/410    513 Sep 28 16:33 1983 man/man1/printenv.1
 rw-r--r--1188/410    991 Nov  2 16:07 1983 man/man1/fmt.1
 rw-r--r--1188/410   2478 Sep 28 16:33 1983 man/man1/ptx.1
 rw-r--r--1188/410    487 Sep 28 16:33 1983 man/man1/pwd.1
 rw-r--r--1188/410   1800 Sep 28 16:33 1983 man/man1/remind.1
 rw-r--r--1188/410   1040 Sep 28 16:33 1983 man/man1/reset.1
 rw-r--r--1188/410    343 Sep 28 16:33 1983 man/man1/rev.1
 rw-r--r--1188/410   1663 Sep 28 16:33 1983 man/man1/rm.1
 rw-r--r--1188/410     14 Oct 26 11:09 1983 man/man1/rmdir.1
 rw-r--r--1188/410   5911 Sep 28 16:33 1983 man/man1/sed.1
 rw-r--r--1188/410    499 Sep 28 16:33 1983 man/man1/see.1
 rw-r--r--1188/410  19269 Nov  1 14:40 1983 man/man1/sh.1
 rw-r--r--1188/410    616 Sep 28 16:33 1983 man/man1/size.1
 rw-r--r--1188/410    544 Sep 28 16:33 1983 man/man1/sleep.1
 rw-r--r--1188/410   1001 Sep 28 16:33 1983 man/man1/soelim.1
 rw-r--r--1188/410    583 Sep 28 16:33 1983 man/man1/split.1
 rw-r--r--1188/410    901 Sep 28 16:33 1983 man/man1/strings.1
 rw-r--r--1188/410    655 Sep 28 16:33 1983 man/man1/strip.1
 rw-r--r--1188/410    907 Sep 28 16:33 1983 man/man1/su.1
 rw-r--r--1188/410    536 Sep 28 16:33 1983 man/man1/sum.1
 rw-r--r--1188/410   1178 Sep 28 16:33 1983 man/man1/tail.1
 rw-r--r--1188/410   4356 Sep 28 16:33 1983 man/man1/tar.1
 rw-r--r--1188/410    419 Sep 28 16:33 1983 man/man1/tee.1
 rw-r--r--1188/410   1908 Sep 28 16:33 1983 man/man1/test.1
 rw-r--r--1188/410   1007 Sep 28 16:33 1983 man/man1/time.1
 rw-r--r--1188/410    475 Sep 28 16:33 1983 man/man1/touch.1
 rw-r--r--1188/410   1601 Sep 28 16:34 1983 man/man1/tr.1
 rw-r--r--1188/410   3867 Sep 28 16:34 1983 man/man1/troff.1
 rw-r--r--1188/410    458 Sep 28 16:34 1983 man/man1/true.1
 rw-r--r--1188/410   5300 Sep 28 16:34 1983 man/man1/tset.1
 rw-r--r--1188/410    795 Sep 28 16:34 1983 man/man1/tsort.1
 rw-r--r--1188/410    291 Sep 28 16:34 1983 man/man1/tty.1
 rw-r--r--1188/410   1461 Sep 28 16:34 1983 man/man1/ul.1


                                 433010137-001                     Page 82 of 93


                                  GENIX SOURCE


 rw-r--r--1188/410     19 Oct 26 11:09 1983 man/man1/uncompact.1
 rw-r--r--1188/410   1366 Sep 28 16:34 1983 man/man1/uniq.1
 rw-r--r--1188/410   1869 Sep 28 16:34 1983 man/man1/vi.1
 rw-r--r--1188/410    607 Sep 28 16:34 1983 man/man1/wait.1
 rw-r--r--1188/410   1450 Sep 28 16:34 1983 man/man1/wc.1
 rw-r--r--1188/410   1490 Sep 28 16:34 1983 man/man1/what.1
 rw-r--r--1188/410   1774 Sep 28 16:34 1983 man/man1/whereis.1
 rw-r--r--1188/410    921 Nov  1 16:39 1983 man/man1/which.1
 rw-r--r--1188/410    372 Sep 28 16:34 1983 man/man1/whoami.1
 rw-r--r--1188/410    337 Sep 28 16:34 1983 man/man1/yes.1
 rw-r--r--1188/410   6463 Nov  1 13:27 1983 man/man1/sccs.1
 rw-r--r--1188/410  12415 Sep 28 16:34 1983 man/man1/admin.1
 rw-r--r--1188/410   4001 Sep 28 16:34 1983 man/man1/comb.1
 rw-r--r--1188/410   6780 Nov  1 16:53 1983 man/man1/delta.1
 rw-r--r--1188/410  16441 Nov  1 16:31 1983 man/man1/get.1
 rw-r--r--1188/410   1816 Sep 28 16:34 1983 man/man1/rmdel.1
 rw-r--r--1188/410   1255 Sep 28 16:34 1983 man/man1/sccsdiff.1
 rw-r--r--1188/410   8506 Nov  1 14:53 1983 man/man1/prt.1
 rw-r--r--1188/410   1284 Sep 28 16:34 1983 man/man1/sccshelp.1
 rw-r--r--1188/410   8701 Nov  1 15:24 1983 man/man1/prs.1
 rw-r--r--1188/410   5955 Nov  1 16:07 1983 man/man1/cdc.1
 rw-r--r--1188/410   1537 Sep 28 16:34 1983 man/man1/sact.1
 rw-r--r--1188/410   1608 Sep 28 16:34 1983 man/man1/unget.1
 rw-r--r--1188/410   4251 Sep 28 16:34 1983 man/man1/val.1
 rw-r--r--1188/410   1041 Nov  2 16:53 1983 man/man1/mt.1
 rw-r--r--1188/410   7397 Sep 28 16:34 1983 man/man1/cc.1
 rw-r--r--1188/410   1598 Sep 28 16:34 1983 man/man1/bdiff.1
 rw-r--r--1188/410    331 Sep 28 16:34 1983 man/man1/cb.1
 rw-r--r--1188/410   1122 Sep 28 16:34 1983 man/man1/gets.1
 rw-r--r--1188/410   1107 Sep 28 16:34 1983 man/man1/lastcomm.1
 rw-r--r--1188/410    691 Sep 28 16:34 1983 man/man1/newgrp.1
 rw-r--r--1188/410   3809 Sep 28 16:35 1983 man/man1/sort.1
 rw-r--r--1188/410   1180 Sep 28 16:35 1983 man/man1/calendar.1
 rw-r--r--1188/410     16 Oct 26 11:10 1983 man/man1/egrep.1
 rw-r--r--1188/410   3282 Sep 28 16:35 1983 man/man1/bc.1
 rw-r--r--1188/410   2100 Sep 28 16:35 1983 man/man1/expr.1
 rw-r--r--1188/410   1644 Sep 28 16:35 1983 man/man1/diff3.1
 rw-r--r--1188/410   4924 Sep 28 16:35 1983 man/man1/m4.1
 rw-r--r--1188/410   1184 Sep 28 16:35 1983 man/man1/diction.1
 rw-r--r--1188/410   1437 Sep 28 16:35 1983 man/man1/style.1
 rw-r--r--1188/410   1549 Sep 28 16:35 1983 man/man1/write.1
 rw-r--r--1188/410    559 Sep 28 16:35 1983 man/man1/wall.1
 rw-r--r--1188/410   2658 Sep 28 16:35 1983 man/man1/nm.1
 rw-r--r--1188/410    577 Sep 28 16:35 1983 man/man1/uvers.1
 rw-r--r--1188/410   5434 Sep 28 16:35 1983 man/man1/eqn.1
 rw-r--r--1188/410     15 Oct 26 11:10 1983 man/man1/neqn.1
 rw-r--r--1188/410   4775 Sep 28 16:35 1983 man/man1/tbl.1
 rw-r--r--1188/410   1414 Sep 28 16:35 1983 man/man1/lex.1
 rw-r--r--1188/410    511 Sep 28 16:35 1983 man/man1/rewind.1
 rw-r--r--1188/410   2232 Nov  1 16:59 1983 man/man1/pc.1
 rw-r--r--1188/410     15 Oct 26 11:10 1983 man/man1/lpq.1
 rw-r--r--1188/410     15 Oct 26 11:10 1983 man/man1/lprm.1
 rw-r--r--1188/410   1092 Sep 28 16:35 1983 man/man1/who.1
 rw-r--r--1188/410    363 Sep 28 16:35 1983 man/man1/users.1
 rw-r--r--1188/410    425 Nov  2 16:54 1983 man/man1/prmail.1
 rw-r--r--1188/410    416 Sep 28 16:35 1983 man/man1/burn.1
 rw-r--r--1188/410   2601 Sep 28 16:35 1983 man/man1/spell.1
 rw-r--r--1188/410   2319 Sep 28 16:35 1983 man/man1/man.1


                                 433010137-001                     Page 83 of 93


                                  GENIX SOURCE


 rw-r--r--1188/410    941 Sep 28 16:35 1983 man/man1/apropos.1
 rw-r--r--1188/410    714 Sep 28 16:35 1983 man/man1/whatis.1
 rw-r--r--1188/410   1740 Nov  1 16:43 1983 man/man1/nice.1
 rw-r--r--1188/410   1076 Sep 28 16:35 1983 man/man1/ranlib.1
 rw-r--r--1188/410     21 Oct 26 11:10 1983 man/man1/uudecode.1c
 rw-r--r--1188/410   2967 Sep 28 16:35 1983 man/man1/uucp.1c
 rw-r--r--1188/410   1736 Oct 14 16:16 1983 man/man1/prof.1
 rw-r--r--1188/410    707 Sep 28 16:35 1983 man/man1/su1.1
 rw-r--r--1188/410    578 Sep 28 16:35 1983 man/man1/trman.1
 rw-r--r--1188/410   2876 Sep 28 16:35 1983 man/man1/vmstat.1
 rw-r--r--1188/410    536 Nov  2 16:55 1983 man/man1/tabs.1
 rw-r--r--1188/410    534 Oct 14 15:45 1983 man/man1/from.1
 rw-r--r--1188/410   3432 Sep 28 16:35 1983 man/man1/msgs.1
 rw-r--r--1188/410    378 Sep 28 16:35 1983 man/man1/stat.1
 rw-r--r--1188/410    293 Sep 28 16:35 1983 man/man1/uid.1
 rw-r--r--1188/410   3263 Sep 28 16:35 1983 man/man1/lint.1
 rw-r--r--1188/410   1234 Sep 28 16:36 1983 man/man1/deroff.1
 rw-r--r--1188/410  15101 Sep 28 16:36 1983 man/man1/mail.1
 rw-r--r--1188/410    344 Sep 28 16:36 1983 man/man1/help.1
 rw-r--r--1188/410     17 Oct 26 11:10 1983 man/man1/enroll.1
 rw-r--r--1188/410     17 Oct 26 11:10 1983 man/man1/xget.1
 rw-r--r--1188/410     19 Oct 26 11:10 1983 man/man1/explain.1
 rw-r--r--1188/410   1344 Sep 28 16:36 1983 man/man1/uusend.1c
 rw-r--r--1188/410   1908 Sep 28 16:36 1983 man/man1/uuencode.1c
 rw-r--r--1188/410   1032 Sep 28 16:36 1983 man/man1/uuclean.1c
 rw-r--r--1188/410   2126 Sep 28 16:36 1983 man/man1/uux.1c
 rw-r--r--1188/410     17 Oct 26 11:10 1983 man/man1/uulog.1c
 rw-r--r--1188/410   1659 Sep 28 16:36 1983 man/man1/units.1
 rw-r--r--1188/410   1092 Sep 28 16:36 1983 man/man1/xsend.1
 rw-r--r--1188/410   2992 Sep 28 16:36 1983 man/man1/cref.1
 rw-r--r--1188/410    466 Sep 28 16:36 1983 man/man1/uptime.1
 rw-r--r--1188/410   1960 Sep 28 16:36 1983 man/man1/yacc.1
 rw-r--r--1188/410     15 Oct 31 10:06 1983 man/man1/checkeq.1
 rwxr-xr-x1188/410      0 Dec  2 18:29 1983 man/man2/
 rw-r--r--1188/410   1093 Oct 25 16:42 1983 man/man2/vhangup.2v
 rw-r--r--1188/410   1193 Sep 28 16:49 1983 man/man2/access.2
 rw-r--r--1188/410    936 Sep 28 16:49 1983 man/man2/acct.2
 rw-r--r--1188/410    973 Sep 28 16:49 1983 man/man2/alarm.2
 rw-r--r--1188/410   1184 Sep 28 16:49 1983 man/man2/brk.2
 rw-r--r--1188/410   1277 Sep 28 16:50 1983 man/man2/read.2
 rw-r--r--1188/410    606 Sep 28 16:49 1983 man/man2/chdir.2
 rw-r--r--1188/410   1618 Sep 28 16:49 1983 man/man2/chmod.2
 rw-r--r--1188/410    817 Sep 28 16:49 1983 man/man2/chown.2
 rw-r--r--1188/410    867 Sep 28 16:49 1983 man/man2/close.2
 rw-r--r--1188/410   1380 Sep 28 16:49 1983 man/man2/creat.2
 rw-r--r--1188/410    979 Sep 28 16:49 1983 man/man2/dup.2
 rw-r--r--1188/410   4864 Sep 28 16:49 1983 man/man2/exec.2
 rw-r--r--1188/410    803 Sep 28 16:49 1983 man/man2/exit.2
 rw-r--r--1188/410   1261 Sep 28 16:49 1983 man/man2/fork.2
 rw-r--r--1188/410     20 Sep 28 16:49 1983 man/man2/getpgrp.2j
 rw-r--r--1188/410    338 Sep 28 16:49 1983 man/man2/getpid.2
 rw-r--r--1188/410    723 Sep 28 16:49 1983 man/man2/getuid.2
 rw-r--r--1188/410   8469 Oct 28 11:10 1983 man/man2/intro.2
 rw-r--r--1188/410   2346 Sep 28 16:49 1983 man/man2/ioctl.2
 rw-r--r--1188/410   1259 Sep 28 16:49 1983 man/man2/kill.2
 rw-r--r--1188/410   1766 Sep 28 16:49 1983 man/man2/killpg.2j
 rw-r--r--1188/410    790 Sep 28 16:49 1983 man/man2/link.2
 rw-r--r--1188/410   1143 Sep 28 16:49 1983 man/man2/lseek.2


                                 433010137-001                     Page 84 of 93


                                  GENIX SOURCE


 rw-r--r--1188/410    945 Sep 28 16:50 1983 man/man2/mknod.2
 rw-r--r--1188/410   1966 Sep 28 16:50 1983 man/man2/mount.2
 rw-r--r--1188/410    947 Sep 28 16:50 1983 man/man2/nice.2
 rw-r--r--1188/410    914 Sep 28 16:50 1983 man/man2/open.2
 rw-r--r--1188/410    488 Sep 28 16:50 1983 man/man2/pause.2
 rw-r--r--1188/410   1344 Sep 28 16:50 1983 man/man2/profil.2
 rw-r--r--1188/410   6686 Oct 28 11:29 1983 man/man2/ptrace.2
 rw-r--r--1188/410   2232 Sep 28 16:50 1983 man/man2/reboot.2v
 rw-r--r--1188/410   1557 Sep 28 16:50 1983 man/man2/setpgrp.2j
 rw-r--r--1188/410    542 Sep 28 16:50 1983 man/man2/setuid.2
 rw-r--r--1188/410   5478 Oct 20 18:48 1983 man/man2/signal.2
 rw-r--r--1188/410   7939 Oct 31 14:36 1983 man/man2/sigsys.2j
 rw-r--r--1188/410   2006 Sep 28 16:51 1983 man/man2/stat.2
 rw-r--r--1188/410    492 Sep 28 16:51 1983 man/man2/stime.2
 rw-r--r--1188/410    601 Sep 28 16:51 1983 man/man2/sync.2
 rw-r--r--1188/410    627 Sep 28 16:51 1983 man/man2/syscall.2
 rw-r--r--1188/410   1024 Sep 28 16:51 1983 man/man2/time.2
 rw-r--r--1188/410    720 Sep 28 16:51 1983 man/man2/times.2
 rw-r--r--1188/410    783 Sep 28 16:51 1983 man/man2/umask.2
 rw-r--r--1188/410    947 Sep 28 16:52 1983 man/man2/unlink.2
 rw-r--r--1188/410    558 Sep 28 16:52 1983 man/man2/utime.2
 rw-r--r--1188/410   2581 Oct 21 10:09 1983 man/man2/vtimes.2v
 rw-r--r--1188/410   2686 Sep 28 16:52 1983 man/man2/vlimit.2v
 rw-r--r--1188/410   2124 Oct 27 09:42 1983 man/man2/vspy.2v
 rw-r--r--1188/410   3197 Oct 31 15:57 1983 man/man2/vread.2v
 rw-r--r--1188/410   1512 Oct 27 09:43 1983 man/man2/vlock.2v
 rw-r--r--1188/410   1258 Oct 31 15:58 1983 man/man2/vwrite.2v
 rw-r--r--1188/410   1610 Sep 28 16:53 1983 man/man2/wait.2
 rw-r--r--1188/410   1971 Sep 28 16:53 1983 man/man2/wait3.2j
 rw-r--r--1188/410   1341 Sep 28 16:53 1983 man/man2/write.2
 rw-r--r--1188/410   1181 Sep 28 17:02 1983 man/man2/gethostname.2
 rw-r--r--1188/410   2335 Sep 28 16:53 1983 man/man2/vfork.2v
 rw-r--r--1188/410   1319 Sep 28 16:53 1983 man/man2/pipe.2
 rw-r--r--1188/410   1653 Oct 31 16:07 1983 man/man2/vmap.2v
 rwxr-xr-x1188/410      0 Dec  2 19:04 1983 man/man3/
 rw-r--r--1188/410     18 Oct 26 11:27 1983 man/man3/signal.3
 rw-r--r--1188/410    429 Sep 28 16:51 1983 man/man3/abort.3
 rw-r--r--1188/410    350 Sep 28 16:51 1983 man/man3/abs.3
 rw-r--r--1188/410    680 Sep 28 16:51 1983 man/man3/assert.3x
 rw-r--r--1188/410    877 Sep 28 16:51 1983 man/man3/atof.3
 rw-r--r--1188/410   1823 Sep 28 16:51 1983 man/man3/crypt.3
 rw-r--r--1188/410   2255 Sep 28 16:51 1983 man/man3/ctime.3
 rw-r--r--1188/410   1224 Sep 28 16:51 1983 man/man3/ctype.3
 rw-r--r--1188/410     19 Oct 26 11:27 1983 man/man3/termlib.3
 rw-r--r--1188/410    827 Sep 28 16:51 1983 man/man3/end.3
 rw-r--r--1188/410   1456 Sep 28 16:52 1983 man/man3/epdtoadr.3
 rw-r--r--1188/410   1081 Sep 28 16:52 1983 man/man3/exp.3m
 rw-r--r--1188/410    860 Sep 28 16:52 1983 man/man3/fclose.3s
 rw-r--r--1188/410    989 Sep 28 16:52 1983 man/man3/ferror.3s
 rw-r--r--1188/410    536 Sep 28 16:52 1983 man/man3/floor.3m
 rw-r--r--1188/410   2049 Nov  1 13:08 1983 man/man3/fopen.3s
 rw-r--r--1188/410   1068 Sep 28 16:52 1983 man/man3/fread.3s
 rw-r--r--1188/410    756 Sep 28 16:52 1983 man/man3/frexp.3
 rw-r--r--1188/410   1112 Sep 28 16:53 1983 man/man3/fseek.3s
 rw-r--r--1188/410    651 Sep 28 16:53 1983 man/man3/gamma.3m
 rw-r--r--1188/410   1549 Sep 28 16:53 1983 man/man3/getc.3s
 rw-r--r--1188/410    431 Sep 28 16:53 1983 man/man3/getenv.3
 rw-r--r--1188/410   1424 Sep 28 16:53 1983 man/man3/getfsent.3


                                 433010137-001                     Page 85 of 93


                                  GENIX SOURCE


 rw-r--r-x1188/410   3155 Dec  2 19:04 1983 man/man3/directory.3s
 rw-r--r--1188/410   1630 Sep 28 16:53 1983 man/man3/getgrent.3
 rw-r--r--1188/410    846 Sep 28 16:53 1983 man/man3/getlogin.3
 rw-r--r--1188/410    620 Sep 28 16:54 1983 man/man3/getpass.3
 rw-r--r--1188/410    508 Nov  1 13:09 1983 man/man3/getpw.3
 rw-r--r--1188/410   1248 Sep 28 16:54 1983 man/man3/getpwent.3
 rw-r--r--1188/410   1024 Sep 28 16:54 1983 man/man3/gets.3s
 rw-r--r--1188/410    435 Sep 28 16:54 1983 man/man3/hypot.3m
 rw-r--r--1188/410   2255 Sep 28 16:54 1983 man/man3/intro.3
 rw-r--r--1188/410    642 Sep 28 16:54 1983 man/man3/j0.3m
 rw-r--r--1188/410  12373 Sep 28 16:54 1983 man/man3/jobs.3j
 rw-r--r--1188/410    699 Sep 28 16:54 1983 man/man3/l3tol.3
 rw-r--r--1188/410   2668 Sep 28 16:54 1983 man/man3/malloc.3
 rw-r--r--1188/410    484 Sep 28 16:54 1983 man/man3/mktemp.3
 rw-r--r--1188/410   1843 Sep 28 16:54 1983 man/man3/monitor.3
 rw-r--r--1188/410   1261 Sep 28 16:54 1983 man/man3/nlist.3
 rw-r--r--1188/410   1165 Sep 28 16:55 1983 man/man3/perror.3
 rw-r--r--1188/410   1527 Sep 28 16:55 1983 man/man3/popen.3s
 rw-r--r--1188/410   5045 Sep 28 16:55 1983 man/man3/printf.3s
 rw-r--r--1188/410   1948 Sep 28 16:55 1983 man/man3/putc.3s
 rw-r--r--1188/410    749 Sep 28 16:55 1983 man/man3/puts.3s
 rw-r--r--1188/410    769 Sep 28 16:55 1983 man/man3/qsort.3
 rw-r--r--1188/410    586 Sep 28 16:55 1983 man/man3/rand.3
 rw-r--r--1188/410   1475 Nov  1 13:11 1983 man/man3/regex.3
 rw-r--r--1188/410   5698 Sep 28 16:55 1983 man/man3/scanf.3s
 rw-r--r--1188/410   1193 Sep 28 16:55 1983 man/man3/setbuf.3s
 rw-r--r--1188/410   1076 Sep 28 16:55 1983 man/man3/setjmp.3
 rw-r--r--1188/410   3011 Nov  1 13:05 1983 man/man3/dbm.3x
 rw-r--r--1188/410   3271 Sep 28 16:55 1983 man/man3/sigset.3
 rw-r--r--1188/410   1195 Sep 28 16:55 1983 man/man3/sin.3m
 rw-r--r--1188/410    494 Sep 28 16:55 1983 man/man3/sinh.3m
 rw-r--r--1188/410    875 Sep 28 16:56 1983 man/man3/sleep.3
 rw-r--r--1188/410   2811 Sep 28 16:56 1983 man/man3/stdio.3s
 rw-r--r--1188/410   1783 Sep 28 16:56 1983 man/man3/string.3
 rw-r--r--1188/410    439 Sep 28 16:56 1983 man/man3/swab.3
 rw-r--r--1188/410    557 Sep 28 16:56 1983 man/man3/system.3
 rw-r--r--1188/410   3499 Sep 28 16:56 1983 man/man3/termcap.3
 rw-r--r--1188/410   1255 Nov  1 13:10 1983 man/man3/plot.3x
 rw-r--r--1188/410    981 Sep 28 16:56 1983 man/man3/ttyname.3
 rw-r--r--1188/410    780 Sep 28 16:56 1983 man/man3/ungetc.3s
 rw-r--r--1188/410    689 Sep 28 16:56 1983 man/man3/valloc.3
 rw-r--r--1188/410   2143 Sep 28 16:56 1983 man/man3/varargs.3
 rw-r--r--1188/410   1365 Nov  2 17:23 1983 man/man3/scandir.3
 rw-r--r--1188/410   3386 Sep 28 16:56 1983 man/man3/curses.3
 rw-r--r--1188/410   1494 Sep 28 16:56 1983 man/man3/openp.3x
 rw-r--r--1188/410   2960 Nov  3 16:21 1983 man/man3/walkdir.3
 rwxr-xr-x1188/410      0 Nov 28 15:31 1983 man/man4/
 rw-r--r--1188/410   1273 Oct 28 11:44 1983 man/man4/cons.4
 rw-r--r--1188/410   1603 Oct 20 18:15 1983 man/man4/mem.4
 rw-r--r--1188/410   2760 Oct 24 14:32 1983 man/man4/dc.4
 rw-r--r--1188/410  29950 Oct 28 15:40 1983 man/man4/tty.4
 rw-r--r--1188/410    644 Oct 31 16:10 1983 man/man4/drum.4
 rw-r--r--1188/410    414 Oct 20 19:11 1983 man/man4/intro.4
 rw-r--r--1188/410   1170 Sep 28 16:52 1983 man/man4/lp.4
 rw-r--r--1188/410   1943 Nov  1 12:44 1983 man/man4/sio.4
 rw-r--r--1188/410   3362 Oct 31 16:11 1983 man/man4/newtty.4
 rw-r--r--1188/410    271 Sep 28 16:52 1983 man/man4/null.4
 rw-r--r--1188/410   2421 Sep 28 16:52 1983 man/man4/pty.4


                                 433010137-001                     Page 86 of 93


                                  GENIX SOURCE


 rw-r--r--1188/410   7407 Sep 28 16:52 1983 man/man4/tc.4
 rwxr-xr-x1188/410      0 Nov 28 15:32 1983 man/man5/
 rw-r--r--1188/410   1139 Oct 24 13:18 1983 man/man5/core.5
 rw-r--r--1188/410   1292 Sep 28 16:52 1983 man/man5/aliases.5
 rw-r--r--1188/410  36241 Nov  1 14:46 1983 man/man5/stab.5
 rw-r--r--1188/410   3083 Sep 28 16:53 1983 man/man5/fstab.5
 rw-r--r--1188/410    867 Sep 28 16:53 1983 man/man5/group.5
 rw-r--r--1188/410    786 Sep 28 16:53 1983 man/man5/mtab.5
 rw-r--r--1188/410   1549 Sep 28 16:53 1983 man/man5/passwd.5
 rw-r--r--1188/410  25287 Oct 27 13:04 1983 man/man5/termcap.5
 rw-r--r--1188/410  11353 Sep 28 16:53 1983 man/man5/a.out.5
 rw-r--r--1188/410   1741 Sep 28 16:54 1983 man/man5/ar.5
 rw-r--r--1188/410   6525 Nov  1 12:35 1983 man/man5/sccsfile.5
 rw-r--r--1188/410    993 Sep 28 16:54 1983 man/man5/dir.5
 rw-r--r--1188/410   6670 Sep 28 16:54 1983 man/man5/filsys.5
 rw-r--r--1188/410   1041 Sep 28 16:54 1983 man/man5/ttys.5
 rw-r--r--1188/410    689 Sep 28 16:54 1983 man/man5/ttytype.5
 rw-r--r--1188/410   1307 Sep 28 16:54 1983 man/man5/utmp.5
 rw-r--r--1188/410    871 Sep 28 16:54 1983 man/man5/wtmp.5
 rw-r--r--1188/410   1775 Sep 28 16:54 1983 man/man5/environ.5
 rw-r--r--1188/410   1954 Sep 28 16:54 1983 man/man5/types.5
 rw-r--r--1188/410   4117 Sep 28 16:54 1983 man/man5/dump.5
 rw-r--r--1188/410   1537 Sep 28 16:54 1983 man/man5/acct.5
 rw-r--r--1188/410   3009 Oct 26 18:07 1983 man/man5/disk.5
 rw-r--r--1188/410   2739 Sep 28 16:55 1983 man/man5/plot.5
 rw-r--r--1188/410   1749 Sep 28 16:55 1983 man/man5/uuencode.5
 rw-r--r--1188/410   2455 Sep 28 16:55 1983 man/man5/gettytab.5
 rwxr-xr-x1188/410      0 Nov 28 15:32 1983 man/man6/
 rw-r--r--1188/410   1092 Oct 17 13:05 1983 man/man6/fortune.6
 rw-r--r--1188/410    644 Sep 28 16:52 1983 man/man6/adventure.6
 rw-r--r--1188/410    283 Sep 28 16:52 1983 man/man6/bcd.6
 rw-r--r--1188/410    524 Sep 28 16:52 1983 man/man6/rain.6
 rw-r--r--1188/410   1200 Sep 28 16:52 1983 man/man6/worm.6
 rw-r--r--1188/410   1527 Sep 28 16:53 1983 man/man6/worms.6
 rw-r--r--1188/410    832 Sep 28 16:53 1983 man/man6/wump.6
 rw-r--r--1188/410   1779 Sep 28 16:53 1983 man/man6/rogue.6
 rw-r--r--1188/410   1571 Sep 28 16:53 1983 man/man6/hangman.6
 rw-r--r--1188/410    291 Sep 28 16:53 1983 man/man6/backgammon.6
 rw-r--r--1188/410   1157 Sep 28 16:53 1983 man/man6/banner.6
 rw-r--r--1188/410   1331 Sep 28 16:53 1983 man/man6/fish.6
 rw-r--r--1188/410  10358 Sep 28 16:53 1983 man/man6/mille.6
 rw-r--r--1188/410    450 Sep 28 16:54 1983 man/man6/number.6
 rw-r--r--1188/410   1736 Sep 28 16:54 1983 man/man6/quiz.6
 rwxr-xr-x1188/410      0 Nov 28 15:32 1983 man/man7/
 rw-r--r--1188/410   4615 Nov  2 17:04 1983 man/man7/ascii.7
 rw-r--r--1188/410   6388 Sep 28 16:59 1983 man/man7/ms.7
 rw-r--r--1188/410   1401 Sep 28 16:59 1983 man/man7/term.7
 rw-r--r--1188/410   8073 Sep 28 16:59 1983 man/man7/hier.7
 rw-r--r--1188/410   4480 Sep 28 16:59 1983 man/man7/eqnchar.7
 rw-r--r--1188/410   1078 Sep 28 16:59 1983 man/man7/greek.7
 rw-r--r--1188/410   5398 Sep 28 16:59 1983 man/man7/me.7
 rw-r--r--1188/410   3362 Sep 28 16:59 1983 man/man7/man.7
 rwxr-xr-x1188/410      0 Dec  2 18:33 1983 man/man8/
 rw-r--r--1188/410   1459 Sep 28 18:49 1983 man/man8/sticky.8
 rw-r--r--1188/410   1611 Oct 28 13:22 1983 man/man8/setroot.8
 rw-r--r--1188/410   1348 Sep 28 18:49 1983 man/man8/cron.8
 rw-r--r--1188/410   1778 Sep 28 18:49 1983 man/man8/dcheck.8
 rw-r--r--1188/410   7032 Sep 28 18:49 1983 man/man8/fsck.8


                                 433010137-001                     Page 87 of 93


                                  GENIX SOURCE


 rw-r--r--1188/410   2952 Sep 28 18:49 1983 man/man8/icheck.8
 rw-r--r--1188/410   4781 Sep 28 18:49 1983 man/man8/init.8
 rw-r--r--1188/410   3448 Sep 28 18:49 1983 man/man8/mkfs.8
 rw-r--r--1188/410    703 Sep 28 18:49 1983 man/man8/mknod.8
 rw-r--r--1188/410   1691 Sep 28 18:49 1983 man/man8/mount.8
 rw-r--r--1188/410     17 Sep 28 18:49 1983 man/man8/chgrp.8
 rw-r--r--1188/410    729 Sep 28 18:49 1983 man/man8/chown.8
 rw-r--r--1188/410   1215 Oct 24 14:39 1983 man/man8/clri.8
 rw-r--r--1188/410   1207 Sep 28 18:49 1983 man/man8/ncheck.8
 rw-r--r--1188/410    960 Sep 28 18:49 1983 man/man8/quot.8
 rw-r--r--1188/410    501 Sep 28 18:49 1983 man/man8/sync.8
 rw-r--r--1188/410    897 Sep 28 18:49 1983 man/man8/update.8
 rw-r--r--1188/410   5979 Oct 28 11:50 1983 man/man8/dump.8
 rw-r--r--1188/410    860 Sep 28 18:49 1983 man/man8/dumpdir.8
 rw-r--r--1188/410   3128 Sep 28 18:49 1983 man/man8/restor.8
 rw-r--r--1188/410   1639 Oct 28 13:12 1983 man/man8/mkpt.8
 rw-r--r--1188/410   1230 Sep 28 18:50 1983 man/man8/catman.8
 rw-r--r--1188/410   2329 Sep 28 18:50 1983 man/man8/badsect.8
 rw-r--r--1188/410    690 Sep 28 18:50 1983 man/man8/vipw.8
 rw-r--r--1188/410    886 Sep 28 18:50 1983 man/man8/renice.8
 rw-r--r--1188/410   6252 Oct 28 13:21 1983 man/man8/pstat.8
 rw-r--r--1188/410    636 Sep 28 18:50 1983 man/man8/halt.8
 rw-r--r--1188/410   2969 Sep 28 18:50 1983 man/man8/reboot.8
 rw-r--r--1188/410   2264 Sep 28 18:50 1983 man/man8/shutdown.8
 rw-r--r--1188/410   5233 Oct 21 11:25 1983 man/man8/delivermail.8
 rw-r--r--1188/410   1383 Sep 28 18:50 1983 man/man8/savecore.8
 rw-r--r--1188/410    574 Sep 28 18:51 1983 man/man8/mklost+found.8
 rw-r--r--1188/410   2486 Sep 28 18:50 1983 man/man8/adduser.8
 rw-r--r--1188/410   1245 Sep 28 18:50 1983 man/man8/dmesg.8
 rw-r--r--1188/410     14 Sep 28 18:50 1983 man/man8/accton.8
 rw-r--r--1188/410   4227 Sep 28 18:50 1983 man/man8/crash.8
 rw-r--r--1188/410    534 Nov  1 14:44 1983 man/man8/analyze.8
 rw-r--r--1188/410   3273 Oct 31 14:25 1983 man/man8/sa.8
 rw-r--r--1188/410   1182 Sep 28 18:50 1983 man/man8/ac.8
 rw-r--r--1188/410    527 Oct 28 11:46 1983 man/man8/dcerr.8
 rw-r--r--1188/410    566 Oct 21 11:03 1983 man/man8/tcerr.8
 rw-r--r--1188/410   1538 Sep 28 18:50 1983 man/man8/makekey.8
 rw-r--r--1188/410    493 Oct 28 13:19 1983 man/man8/mmerr.8
 rw-r--r--1188/410   1197 Sep 28 18:50 1983 man/man8/rc.8
 rw-r--r--1188/410   3075 Oct 21 11:27 1983 man/man8/getty.8
 rw-r--r--1188/410    867 Oct 26 18:55 1983 man/man8/dcutest.8
 rwxr-xr-x1188/410      6 Jan 25 18:19 1983 man/break
 rwxr-xr-x1188/410   1394 Nov  1 15:09 1983 man/tocrc
 rwxr-xr-x1188/410      0 Nov 28 15:33 1983 man/man0/
 rw-r--r--1188/410    237 Mar  4 17:19 1983 man/man0/template.0
 rw-r--r--1188/410   7665 Nov  3 17:40 1983 man/man0/toc1
 rw-r--r--1188/410   8397 Nov  3 17:35 1983 man/man0/tocx1
 rw-r--r--1188/410   2663 Nov  3 17:35 1983 man/man0/tocx2
 rw-r--r--1188/410   3951 Nov  3 17:36 1983 man/man0/tocx3
 rw-r--r--1188/410    474 Nov  3 17:36 1983 man/man0/tocx4
 rw-r--r--1188/410   1126 Nov  3 17:36 1983 man/man0/tocx5
 rw-r--r--1188/410    686 Nov  3 17:36 1983 man/man0/tocx6
 rw-r--r--1188/410    357 Nov  3 17:36 1983 man/man0/tocx7
 rw-r--r--1188/410   2181 Nov  3 17:36 1983 man/man0/tocx8
 rw-r--r--1188/410 143765 Nov  3 17:40 1983 man/man0/ptxx
 rw-r--r--1188/410   2288 Nov  3 17:40 1983 man/man0/toc2
 rw-r--r--1188/410   2750 Nov  3 17:40 1983 man/man0/toc3
 rw-r--r--1188/410    451 Nov  3 17:40 1983 man/man0/toc4


                                 433010137-001                     Page 88 of 93


                                  GENIX SOURCE


 rw-r--r--1188/410   1022 Nov  3 17:40 1983 man/man0/toc5
 rw-r--r--1188/410    633 Nov  3 17:40 1983 man/man0/toc6
 rw-r--r--1188/410    336 Nov  3 17:40 1983 man/man0/toc7
 rw-r--r--1188/410   2004 Nov  3 17:40 1983 man/man0/toc8
 rw-r--r--1188/410   1051 Nov  3 17:34 1983 man/man0/ptx.in
 rw-r--r--1188/410   2133 Jan 25 18:22 1983 man/man0/cshcmd
 rw-r--r--1188/410    402 Jan 25 18:22 1983 man/man0/ignore
 rw-r--r--1188/410  15313 Jan 25 18:45 1983 man/man0/tmpcattoc
 rw-r--r--1188/410    976 Mar  1 16:51 1983 man/man0/m1.sed
 rw-r--r--1188/410    437 Mar  1 16:51 1983 man/man0/m2.sed
 rw-r--r--1188/410    641 Mar  1 16:51 1983 man/man0/trref.sed
 rw-r--r--1188/410   1117 Nov  3 17:34 1983 man/man0/toc.in
 rw-r--r--1188/410  48578 Nov  1 17:14 1983 man/man0/toc.q
 rw-r--r--1188/410 278372 Nov  1 17:37 1983 man/man0/ptx.q
 rw-r--r--1188/410  16376 Nov  7 14:27 1983 man/man0/intro
 rwxr-xr-x1188/410      0 Dec  1 14:22 1983 doc/
 rw-r--r--1188/410 292044 Dec 19 12:44 1983 doc/release.lpr
 rw-r--r--1188/410  42277 Nov 29 17:18 1983 doc/implement
 rw-r--r--1188/410   9935 Nov 30 15:52 1983 doc/makefile
 rwxr-xr-x1188/410      0 Dec  1 14:20 1983 doc/curses/
 rw-r--r--1188/410   1576 Nov 29 19:30 1983 doc/curses/makefile
 rw-r--r--1188/410    309 Nov 29 18:43 1983 doc/curses/Master
 rw-r--r--1188/410    173 Nov 14 13:41 1983 doc/curses/Spellit
 rw-r--r--1188/410   3719 Nov 29 18:45 1983 doc/curses/appen.A
 rw-r--r--1188/410  19893 Nov 29 18:45 1983 doc/curses/appen.A.tbl
 rw-r--r--1188/410   2271 Nov 29 18:45 1983 doc/curses/appen.B
 rw-r--r--1188/410    430 Nov 29 18:46 1983 doc/curses/c_macros
 rw-r--r--1188/410    431 Nov 14 13:42 1983 doc/curses/disclaim
 rw-r--r--1188/410   5242 Nov 14 14:08 1983 doc/curses/doc.I
 rw-r--r--1188/410   1713 Nov 14 13:52 1983 doc/curses/doc.II
 rw-r--r--1188/410   4797 Nov 14 13:52 1983 doc/curses/doc.III
 rw-r--r--1188/410   1518 Nov 14 13:43 1983 doc/curses/doc.IV
 rw-r--r--1188/410   1447 Nov 14 13:45 1983 doc/curses/intro.0
 rw-r--r--1188/410   5990 Nov 29 18:49 1983 doc/curses/intro.1
 rw-r--r--1188/410   1242 Nov 29 18:50 1983 doc/curses/intro.2
 rw-r--r--1188/410   4745 Nov 14 13:44 1983 doc/curses/intro.3
 rw-r--r--1188/410   5237 Nov 29 19:09 1983 doc/curses/intro.4
 rw-r--r--1188/410    672 Nov 14 13:44 1983 doc/curses/intro.5
 rw-r--r--1188/410   2868 Nov 29 19:11 1983 doc/curses/life.c
 rw-r--r--1188/410    893 Nov 29 19:24 1983 doc/curses/macros
 rw-r--r--1188/410   1230 Nov 14 13:45 1983 doc/curses/macros.coms
 rw-r--r--1188/410   2348 Nov 14 13:45 1983 doc/curses/twinkle1.c
 rw-r--r--1188/410   1273 Nov 14 13:46 1983 doc/curses/twinkle2.c
 rw-r--r--1188/410    422 Nov 14 13:46 1983 doc/curses/win_st.c
 rw-r--r--1188/410   2299 Nov 29 18:45 1983 doc/curses/appen.C
 rw-r--r--1188/410   7419 Nov 29 19:07 1983 doc/curses/intro.2.tbl
 rwxr-xr-x1188/410      0 Dec  1 14:20 1983 doc/troff/
 rw-r--r--1188/410   2648 Jun 14 11:39 1983 doc/troff/add
 rw-r--r--1188/410   6840 Nov 29 20:16 1983 doc/troff/m0
 rw-r--r--1188/410  14448 Nov 29 20:16 1983 doc/troff/m0a
 rw-r--r--1188/410  24708 Nov 29 20:16 1983 doc/troff/m1
 rw-r--r--1188/410  14195 Nov 29 20:16 1983 doc/troff/m2
 rw-r--r--1188/410  16830 Nov 29 20:16 1983 doc/troff/m3
 rw-r--r--1188/410  13328 Nov 29 20:16 1983 doc/troff/m4
 rw-r--r--1188/410  12562 Nov 29 20:16 1983 doc/troff/m5
 rw-r--r--1188/410   1514 Nov 29 20:18 1983 doc/troff/table1
 rw-r--r--1188/410   4291 Nov 29 20:18 1983 doc/troff/table2
 rw-r--r--1188/410    170 Feb 12 20:27 1979 doc/troff/tprint


                                 433010137-001                     Page 89 of 93


                                  GENIX SOURCE


 rwxr-xr--1188/410    260 Jun 14 11:41 1983 doc/troff/nprint
 rwxr-xr--1188/410    240 Oct 12 14:32 1983 doc/troff/tprintq
 rw-r--r--1188/410  19051 Nov 29 17:14 1983 doc/db16supt
 rw-r--r--1188/410  32933 Nov 29 18:20 1983 doc/sccs.me
 rw-r--r--1188/410  27432 Nov 29 17:14 1983 doc/cc
 rwxr-xr-x1188/410      0 Dec  1 14:20 1983 doc/trofftut/
 rw-r--r--1188/410   1561 Nov 29 20:20 1983 doc/trofftut/tt00
 rw-r--r--1188/410   4045 Nov 29 20:20 1983 doc/trofftut/tt01
 rw-r--r--1188/410   4001 Nov 29 20:20 1983 doc/trofftut/tt03
 rw-r--r--1188/410   4585 Nov 29 20:20 1983 doc/trofftut/tt02
 rw-r--r--1188/410   1890 Nov 29 20:20 1983 doc/trofftut/tt05
 rw-r--r--1188/410   3114 Nov 29 20:20 1983 doc/trofftut/tt04
 rw-r--r--1188/410   6850 Nov 29 20:20 1983 doc/trofftut/tt06
 rw-r--r--1188/410   1781 Nov 29 20:20 1983 doc/trofftut/tt07
 rw-r--r--1188/410   5779 Nov 29 20:21 1983 doc/trofftut/tt09
 rw-r--r--1188/410   4640 Nov 29 20:22 1983 doc/trofftut/tt10
 rw-r--r--1188/410   3977 Nov 29 20:22 1983 doc/trofftut/tt11
 rw-r--r--1188/410   2438 Nov 29 20:22 1983 doc/trofftut/tt12
 rw-r--r--1188/410   1659 Nov 29 20:23 1983 doc/trofftut/tt13
 rw-r--r--1188/410   3069 Nov 29 20:22 1983 doc/trofftut/tt14
 rw-r--r--1188/410   1072 Nov 29 20:23 1983 doc/trofftut/ttack
 rw-r--r--1188/410   2211 Nov 29 20:23 1983 doc/trofftut/ttcharset
 rw-r--r--1188/410   3361 Nov 29 20:24 1983 doc/trofftut/ttindex
 rw-r--r--1188/410   1365 Nov 29 20:19 1983 doc/trofftut/tt.mac
 rw-r--r--1188/410   2726 Nov 29 20:20 1983 doc/trofftut/tt08
 rwxr-xr-x1188/410      0 Dec  1 14:20 1983 doc/vmunix/
 rw-r--r--1188/410   1669 Nov 22 13:49 1983 doc/vmunix/copyr.t
 rw-r--r--1188/410   1366 Nov 22 13:49 1983 doc/vmunix/title.t
 rw-r--r--1188/410   8141 Nov 22 13:49 1983 doc/vmunix/contents.t
 rw-r--r--1188/410  50367 Nov 29 17:10 1983 doc/as
 rwxr-xr-x1188/410      0 Dec  1 14:20 1983 doc/uucp/
 rw-r--r--1188/410  41902 Nov 22 13:38 1983 doc/uucp/uucp_impl
 rwxr-xr-x1188/410      0 Dec  1 14:20 1983 doc/eqn/
 rw-r--r--1188/410    866 Oct  7 11:39 1983 doc/eqn/e.mac
 rw-r--r--1188/410   1468 Oct  7 11:39 1983 doc/eqn/e0
 rw-r--r--1188/410   1732 Oct  7 11:39 1983 doc/eqn/e1
 rw-r--r--1188/410   1303 Oct  7 11:39 1983 doc/eqn/e2
 rw-r--r--1188/410   4628 Oct  7 11:39 1983 doc/eqn/e3
 rw-r--r--1188/410   9313 Oct  7 11:40 1983 doc/eqn/e4
 rw-r--r--1188/410   5444 Oct  7 11:40 1983 doc/eqn/e5
 rw-r--r--1188/410   5050 Oct  7 11:40 1983 doc/eqn/e6
 rw-r--r--1188/410   2088 Oct  7 11:40 1983 doc/eqn/e7
 rw-r--r--1188/410    314 Oct  7 11:40 1983 doc/eqn/g.mac
 rw-r--r--1188/410   1898 Oct  7 11:40 1983 doc/eqn/g0
 rw-r--r--1188/410   9889 Oct  7 11:40 1983 doc/eqn/g1
 rw-r--r--1188/410   9357 Oct  7 11:40 1983 doc/eqn/g2
 rw-r--r--1188/410   4313 Oct  7 11:41 1983 doc/eqn/g3
 rw-r--r--1188/410   4845 Nov 29 19:40 1983 doc/eqn/g4
 rw-r--r--1188/410   2030 Oct  7 11:41 1983 doc/eqn/g5
 rw-r--r--1188/410  28769 Nov 29 18:25 1983 doc/vmpaper
 rwxr-xr-x1188/410      0 Dec  1 14:21 1983 doc/csh/
 rw-r--r--1188/410  29867 Nov 29 18:42 1983 doc/csh/csh.1
 rw-r--r--1188/410  37286 Nov 29 18:42 1983 doc/csh/csh.2
 rw-r--r--1188/410  15279 Nov 29 18:42 1983 doc/csh/csh.3
 rw-r--r--1188/410   3933 Nov 29 18:40 1983 doc/csh/csh.4
 rw-r--r--1188/410   1870 Dec 28 16:10 1982 doc/csh/csh.a
 rw-r--r--1188/410  41262 Dec 28 16:10 1982 doc/csh/csh.g
 rw-r--r--1188/410    137 Dec 28 16:10 1982 doc/csh/tabs


                                 433010137-001                     Page 90 of 93


                                  GENIX SOURCE


 rwxr-xr-x1188/410      0 Dec  1 14:21 1983 doc/ex/
 rw-r--r--1188/410  56261 Nov 29 19:42 1983 doc/ex/edit.tut
 rw-r--r--1188/410   1429 Dec 28 16:50 1982 doc/ex/edit.vindex
 rw-r--r--1188/410  57694 Nov 29 19:46 1983 doc/ex/ex.rm
 rw-r--r--1188/410   2546 Nov  8 16:11 1983 doc/ex/makefile
 rw-r--r--1188/410   9848 Nov 29 19:55 1983 doc/ex/vi.summary
 rw-r--r--1188/410  25106 Nov 29 19:55 1983 doc/ex/vi.chars
 rw-r--r--1188/410  78461 Nov 29 19:55 1983 doc/ex/vi.in
 rw-r--r--1188/410  26500 Nov 29 17:11 1983 doc/awk
 rw-r--r--1188/410  16880 Nov 29 17:14 1983 doc/dbmon
 rwxr-xr-x1188/410      0 Dec  1 14:21 1983 doc/mail/
 rw-r--r--1188/410    843 Dec 29 11:44 1982 doc/mail/addenda.nr
 rw-r--r--1188/410    278 Dec 29 11:44 1982 doc/mail/mail0.nr
 rw-r--r--1188/410   1061 Dec 29 11:44 1982 doc/mail/mail1.nr
 rw-r--r--1188/410   3889 Dec 29 11:44 1982 doc/mail/mail2.nr
 rw-r--r--1188/410   5605 Dec 29 11:44 1982 doc/mail/mail3.nr
 rw-r--r--1188/410   1746 Dec 29 11:44 1982 doc/mail/mail4.nr
 rw-r--r--1188/410   1721 Dec 29 11:44 1982 doc/mail/mail5.nr
 rw-r--r--1188/410   4679 Dec 29 11:44 1982 doc/mail/mail6.nr
 rw-r--r--1188/410  11050 Dec 29 11:44 1982 doc/mail/mail7.nr
 rw-r--r--1188/410    360 Dec 29 11:44 1982 doc/mail/mail8.nr
 rw-r--r--1188/410    238 Dec 29 11:44 1982 doc/mail/makefile
 rwxr-xr-x1188/410      0 Dec  1 14:21 1983 doc/refer/
 rw-r--r--1188/410  18839 Nov 13 16:04 1983 doc/refer/pubuse
 rw-r--r--1188/410  40471 Nov 29 20:04 1983 doc/refer/refer
 rwxr-xr-x1188/410      0 Dec  1 14:21 1983 doc/memacros/
 rw-r--r--1188/410  39736 Nov 29 20:02 1983 doc/memacros/intro.nr
 rw-r--r--1188/410  28133 Nov 29 20:03 1983 doc/memacros/ref.nr
 rwxr-xr-x1188/410      0 Dec  1 14:21 1983 doc/msmacros/
 rw-r--r--1188/410  19425 Nov 20 17:24 1983 doc/msmacros/refcard
 rw-r--r--1188/410  20874 Dec 30 16:08 1982 doc/msmacros/ms
 rwxr-xr-x1188/410      0 Dec  1 14:21 1983 doc/summary/
 rw-r--r--1188/410    219 Nov 22 13:29 1983 doc/summary/vhel.mac
 rw-r--r--1188/410   8343 Nov 22 13:29 1983 doc/summary/vhel0
 rw-r--r--1188/410  18447 Nov 22 13:29 1983 doc/summary/vhel1
 rw-r--r--1188/410   3860 Nov 22 13:29 1983 doc/summary/vhel2
 rw-r--r--1188/410   7101 Nov 22 13:29 1983 doc/summary/vhel3
 rw-r--r--1188/410   3106 Nov 22 13:29 1983 doc/summary/vhel4
 rw-r--r--1188/410     70 Nov 22 13:29 1983 doc/summary/vhel5
 rw-r--r--1188/410   1034 Nov 22 13:29 1983 doc/summary/vhel6
 rwxr-xr-x1188/410      0 Dec  1 14:21 1983 doc/cacm/
 rw-r--r--1188/410    268 Nov 29 18:38 1983 doc/cacm/p.mac
 rw-r--r--1188/410  16255 Nov 29 18:39 1983 doc/cacm/p1
 rw-r--r--1188/410  12977 Nov 29 18:37 1983 doc/cacm/p2
 rw-r--r--1188/410   4491 Nov 29 18:37 1983 doc/cacm/p3
 rw-r--r--1188/410  14283 Nov 29 18:37 1983 doc/cacm/p4
 rw-r--r--1188/410   7446 Nov 29 18:37 1983 doc/cacm/p5
 rw-r--r--1188/410   1435 Nov 29 18:39 1983 doc/cacm/p6
 rwxr-xr-x1188/410      0 Dec  1 14:21 1983 doc/beginners/
 rw-r--r--1188/410  13120 Nov 22 13:44 1983 doc/beginners/u1
 rw-r--r--1188/410    671 Nov 22 13:44 1983 doc/beginners/u.mac
 rw-r--r--1188/410   1284 Nov 22 13:44 1983 doc/beginners/u0
 rw-r--r--1188/410  25203 Nov 22 13:44 1983 doc/beginners/u2
 rw-r--r--1188/410   7640 Nov 22 13:44 1983 doc/beginners/u3
 rw-r--r--1188/410   6258 Nov 22 13:44 1983 doc/beginners/u4
 rw-r--r--1188/410   3080 Nov 22 13:44 1983 doc/beginners/u5
 rwxr-xr-x1188/410      0 Dec  1 14:22 1983 doc/edtut/
 rw-r--r--1188/410    711 Nov 29 19:39 1983 doc/edtut/e0


                                 433010137-001                     Page 91 of 93


                                  GENIX SOURCE


 rw-r--r--1188/410   7060 Nov 29 19:39 1983 doc/edtut/e2
 rw-r--r--1188/410   4081 Nov 29 19:39 1983 doc/edtut/e6
 rw-r--r--1188/410   3210 Nov 29 19:39 1983 doc/edtut/e7
 rw-r--r--1188/410    517 Nov 29 19:39 1983 doc/edtut/e.mac
 rw-r--r--1188/410   5012 Nov 29 19:39 1983 doc/edtut/e1
 rw-r--r--1188/410   7195 Nov 29 19:39 1983 doc/edtut/e3
 rw-r--r--1188/410   5685 Nov 29 19:39 1983 doc/edtut/e4
 rw-r--r--1188/410   4469 Nov 29 19:40 1983 doc/edtut/e5
 rwxr-xr-x1188/410      0 Dec  1 14:22 1983 doc/adv.ed/
 rw-r--r--1188/410    950 Nov 29 18:29 1983 doc/adv.ed/ae0
 rw-r--r--1188/410   1435 Nov 29 18:29 1983 doc/adv.ed/ae1
 rw-r--r--1188/410  22106 Nov 29 18:31 1983 doc/adv.ed/ae2
 rw-r--r--1188/410  10785 Nov 29 18:36 1983 doc/adv.ed/ae3
 rw-r--r--1188/410   6021 Nov 29 18:34 1983 doc/adv.ed/ae5
 rw-r--r--1188/410   4237 Nov 29 18:36 1983 doc/adv.ed/ae4
 rw-r--r--1188/410    835 Nov 29 18:29 1983 doc/adv.ed/ae.mac
 rw-r--r--1188/410   9325 Nov 29 18:36 1983 doc/adv.ed/ae6
 rw-r--r--1188/410   4478 Nov 29 18:34 1983 doc/adv.ed/ae7
 rw-r--r--1188/410    497 Nov 29 18:36 1983 doc/adv.ed/ae9
 rwxr-xr-x1188/410      0 Dec  1 14:22 1983 doc/shell/
 rw-r--r--1188/410    611 Nov 29 20:07 1983 doc/shell/t.mac
 rw-r--r--1188/410  10304 Nov 29 20:07 1983 doc/shell/t1
 rw-r--r--1188/410  18494 Nov 29 20:07 1983 doc/shell/t2
 rw-r--r--1188/410  20261 Nov 29 20:07 1983 doc/shell/t3
 rw-r--r--1188/410   2572 Nov 29 20:07 1983 doc/shell/t4
 rw-r--r--1188/410  42202 Nov 29 18:25 1983 doc/tbl
 rw-r--r--1188/410  28826 Nov 29 17:14 1983 doc/bc
 rw-r--r--1188/410  23864 Nov 29 17:15 1983 doc/dc
 rw-r--r--1188/410  28001 Nov 29 17:19 1983 doc/iosys
 rwxr-xr-x1188/410      0 Dec  1 14:22 1983 doc/fsck/
 rw-r--r--1188/410  46813 Nov 29 19:57 1983 doc/fsck/fsck.mm
 rw-r--r--1188/410   1287 Nov 29 19:57 1983 doc/fsck/fsck.rc
 rw-r--r--1188/410  34667 Nov 18 19:12 1983 doc/fsck/fsck.stoc
 rw-r--r--1188/410    743 Nov 13 16:47 1983 doc/fsck/fsck.toc.mac
 rw-r--r--1188/410  18287 Nov 29 18:17 1983 doc/m4
 rw-r--r--1188/410  51453 Nov 29 18:18 1983 doc/lex
 rw-r--r--1188/410  23711 Nov 29 18:18 1983 doc/make
 rw-r--r--1188/410  12053 Nov 29 18:21 1983 doc/security
 rw-r--r--1188/410  27187 Nov 29 18:22 1983 doc/sed
 rw-r--r--1188/410  32501 Nov 29 18:28 1983 doc/lint
 rw-r--r--1188/410  20764 Nov 29 18:28 1983 doc/password
 rwxr-xr-x1188/410      0 Dec  1 14:22 1983 doc/sys16/
 rw-r--r--1188/410    116 Nov 29 20:50 1983 doc/sys16/ch1
 rw-r--r--1188/410    116 Nov 29 20:51 1983 doc/sys16/ch2
 rw-r--r--1188/410    116 Nov 29 20:51 1983 doc/sys16/ch3
 rw-r--r--1188/410    116 Nov 29 20:51 1983 doc/sys16/ch4
 rw-r--r--1188/410  51064 Nov 29 20:51 1983 doc/sys16/ch5
 rw-r--r--1188/410    116 Nov 29 20:51 1983 doc/sys16/ch6
 rw-r--r--1188/410    143 Nov 29 20:49 1983 doc/sys16/ch0
 rw-r--r--1188/410    116 Nov 29 20:51 1983 doc/sys16/ch7
 rwxr-xr-x1188/410      0 Dec  1 14:22 1983 doc/uprog/
 rw-r--r--1188/410    560 Nov 29 20:25 1983 doc/uprog/cwscript
 rw-r--r--1188/410    646 Nov 29 20:27 1983 doc/uprog/p.mac
 rw-r--r--1188/410    928 Nov 29 20:25 1983 doc/uprog/p0
 rw-r--r--1188/410    990 Nov 29 20:28 1983 doc/uprog/p1
 rw-r--r--1188/410   4661 Nov 29 20:30 1983 doc/uprog/p2
 rw-r--r--1188/410   8752 Nov 29 20:30 1983 doc/uprog/p3
 rw-r--r--1188/410  12349 Nov 29 20:30 1983 doc/uprog/p4


                                 433010137-001                     Page 92 of 93


                                  GENIX SOURCE


 rw-r--r--1188/410  12601 Nov 29 20:30 1983 doc/uprog/p5
 rw-r--r--1188/410   8498 Nov 29 20:31 1983 doc/uprog/p6
 rw-r--r--1188/410    400 Nov 29 20:25 1983 doc/uprog/p8
 rw-r--r--1188/410  12966 Nov 29 20:31 1983 doc/uprog/p9
 rwxr-xr-x1188/410      0 Dec  1 14:22 1983 doc/yacc/
 rw-r--r--1188/410   1897 Jan 24 17:56 1983 doc/yacc/ss..
 rw-r--r--1188/410   6505 Jan 24 17:56 1983 doc/yacc/ss0
 rw-r--r--1188/410   4321 Jan 24 17:56 1983 doc/yacc/ss1
 rw-r--r--1188/410   4279 Jan 24 17:56 1983 doc/yacc/ss2
 rw-r--r--1188/410   3424 Jan 24 17:56 1983 doc/yacc/ss3
 rw-r--r--1188/410   9228 Jan 24 17:56 1983 doc/yacc/ss4
 rw-r--r--1188/410   8713 Jan 24 17:56 1983 doc/yacc/ss5
 rw-r--r--1188/410   5457 Jan 24 17:56 1983 doc/yacc/ss6
 rw-r--r--1188/410   4999 Jan 24 17:56 1983 doc/yacc/ss7
 rw-r--r--1188/410   2642 Jan 24 17:56 1983 doc/yacc/ss8
 rw-r--r--1188/410   4924 Jan 24 17:56 1983 doc/yacc/ss9
 rw-r--r--1188/410   5294 Nov 29 20:32 1983 doc/yacc/ssA
 rw-r--r--1188/410    868 Jan 24 17:56 1983 doc/yacc/ssB
 rw-r--r--1188/410   2835 Jan 24 17:56 1983 doc/yacc/ssa
 rw-r--r--1188/410   2636 Nov 29 20:33 1983 doc/yacc/ssb
 rw-r--r--1188/410   8532 Jan 24 17:56 1983 doc/yacc/ssc
 rw-r--r--1188/410   1383 Jan 24 17:56 1983 doc/yacc/ssd
 rwxr-xr-x1188/410      0 Dec  1 14:22 1983 doc/diction/
 rw-r--r--1188/410    368 Nov 29 19:32 1983 doc/diction/ap1
 rw-r--r--1188/410   7823 Nov 29 19:32 1983 doc/diction/ap2
 rw-r--r--1188/410   1121 Nov 29 19:32 1983 doc/diction/rm0
 rw-r--r--1188/410  33363 Nov 29 19:33 1983 doc/diction/rm1
 rw-r--r--1188/410   1947 Nov 29 19:33 1983 doc/diction/rm2
 rwxr-xr-x1188/410      0 Dec  1 14:22 1983 doc/sys5/
 rw-r--r--1188/410   7024 Oct 12 16:50 1983 doc/sys5/sccs_inter
 rw-r--r--1188/410  77973 Nov 29 20:11 1983 doc/sys5/sccs_guide





























                                 433010137-001                     Page 93 of 93

