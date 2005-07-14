#
#    /etc/profile.d/alljava.csh
#
# send feedback to http://www.suse.de/feedback

#
# This script sets some environment variables for default java.
# Affected variables: PATH, JAVA_BINDIR, JAVA_HOME, JRE_HOME, 
#                     JDK_HOME, SDK_HOME
#

set __libdir=lib
if ( -L /usr/lib64/jvm/java || -L /usr/lib64/jvm/jre ) then
  set __libdir=lib64
endif

if ( -x /usr/$__libdir/jvm/java/bin/java || -x /usr/$__libdir/jvm/java/bin/jre ) then
  setenv JAVA_BINDIR /usr/$__libdir/jvm/java/bin
  setenv JAVA_ROOT /usr/$__libdir/jvm/java
  setenv JAVA_HOME /usr/$__libdir/jvm/java
  if ( -x /usr/$__libdir/jvm/java/jre/bin/java ) then
    setenv JRE_HOME /usr/$__libdir/jvm/java/jre
  else
    setenv JRE_HOME /usr/$__libdir/jvm/java
  endif        
  unsetenv JDK_HOME
  unsetenv SDK_HOME
  if ( -x /usr/$__libdir/jvm/java/bin/javac ) then
    # it is development kit 
    if ( -x /usr/$__libdir/jvm/java/bin/jre ) then
      setenv JDK_HOME /usr/$__libdir/jvm/java
    else
      setenv JDK_HOME /usr/$__libdir/jvm/java
      setenv SDK_HOME /usr/$__libdir/jvm/java
    endif
  endif
else
  if ( -x /usr/$__libdir/jvm/jre/bin/java ) then
    # it is IBMJava2-JRE or SunJava2-JRE
    setenv JAVA_BINDIR /usr/$__libdir/jvm/jre/bin
    setenv JAVA_ROOT /usr/$__libdir/jvm/jre
    setenv JAVA_HOME /usr/$__libdir/jvm/jre
    setenv JRE_HOME /usr/$__libdir/jvm/jre
    unsetenv JDK_HOME
    unsetenv SDK_HOME
  endif
endif

unset __libdir
