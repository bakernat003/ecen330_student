add_executable(lab6.elf main.c bhTester.c buttonHandler.c flashSequence.c fsTester.c globals.c simonControl.c simonDisplay.c verifySequence.c vsTester.c )
target_link_libraries(lab6.elf ${330_LIBS} intervalTimer buttons_switches)
set_target_properties(lab6.elf PROPERTIES LINKER_LANGUAGE CXX)