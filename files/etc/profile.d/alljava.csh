#
#    /etc/profile.d/alljava.csh
#
# send feedback to http://www.suse.de/feedback

#
# This script sets some environment variables for default java.
# Affected variables: PATH, JAVA_BINDIR, JAVA_HOME, JRE_HOME, 
#                     JDK_HOME, SDK_HOME
#

if ( -x /usr/lib/jvm/java/bin/java || -x /usr/lib/jvm/java/bin/jre ) then
  setenv JAVA_BINDIR /usr/lib/jvm/java/bin
  setenv JAVA_ROOT /usr/lib/jvm/java
  setenv JAVA_HOME /usr/lib/jvm/java
  if ( -x /usr/lib/jvm/java/jre/bin/java ) then
    setenv JRE_HOME /usr/lib/jvm/java/jre
  else
    setenv JRE_HOME /usr/lib/jvm/java
  endif        
  unsetenv JDK_HOME
  unsetenv SDK_HOME
  if ( -x /usr/lib/jvm/java/bin/javac ) then
    # it is development kit 
    if ( -x /usr/lib/jvm/java/bin/jre ) then
      setenv JDK_HOME /usr/lib/jvm/java
    else
      setenv JDK_HOME /usr/lib/jvm/java
      setenv SDK_HOME /usr/lib/jvm/java
    endif
  endif
else
  if ( -x /usr/lib/jvm/java/jre/bin/java ) then
    # it is IBMJava2-JRE or SunJava2-JRE
    setenv JAVA_BINDIR /usr/lib/jvm/java/jre/bin
    setenv JAVA_ROOT /usr/lib/jvm/java
    setenv JAVA_HOME /usr/lib/jvm/java/jre
    setenv JRE_HOME /usr/lib/jvm/java/jre
    unsetenv JDK_HOME
    unsetenv SDK_HOME
  endif
endif
