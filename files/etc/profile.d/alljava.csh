#
#    /etc/profile.d/alljava.csh
#
# send feedback to http:/www.suse.de/feedback

#
# This script sets some environment variables for default java.
# Affected variables: PATH, JAVA_BINDIR, JAVA_HOME, JRE_HOME, 
#                     JDK_HOME, SDK_HOME
#

if ( -x /usr/lib/java/bin/java || -x /usr/lib/java/bin/jre ) then
  setenv PATH ${PATH}:/usr/lib/java/bin
  setenv JAVA_BINDIR /usr/lib/java/bin
  setenv JAVA_ROOT /usr/lib/java
  setenv JAVA_HOME /usr/lib/java
  if ( -x /usr/lib/java/jre/bin/java ) then
    setenv JRE_HOME /usr/lib/java/jre
  else
    setenv JRE_HOME /usr/lib/java
  endif        
  unsetenv JDK_HOME
  unsetenv SDK_HOME
  if ( -x /usr/lib/java/bin/javac ) then
    # it is development kit 
    if ( -x /usr/lib/java/bin/jre ) then
      setenv JDK_HOME /usr/lib/java
    else
      setenv JDK_HOME /usr/lib/java
      setenv SDK_HOME /usr/lib/java
    endif
  endif
else
  if ( -x /usr/lib/java/jre/bin/java ) then
    # it is IBMJava2-JRE or SunJava2-JRE
    setenv PATH ${PATH}:/usr/lib/java/jre/bin
    setenv JAVA_BINDIR /usr/lib/java/jre/bin
    setenv JAVA_ROOT /usr/lib/java
    setenv JAVA_HOME /usr/lib/java/jre
    setenv JRE_HOME /usr/lib/java/jre
    unsetenv JDK_HOME
    unsetenv SDK_HOME
  endif
endif
