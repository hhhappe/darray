# Create a library called "Hello" which includes the source file "hello.cxx".
# The extension is already found.  Any number of sources could be listed here.

ADD_EXECUTABLE(darray-test darray-test)
# ADD_EXECUTABLE(judy-test judy-test)

TARGET_LINK_LIBRARIES(darray-test darray rt)
# TARGET_LINK_LIBRARIES(judy-test Judy rt)
