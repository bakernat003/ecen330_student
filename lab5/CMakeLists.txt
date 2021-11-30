add_executable(lab5.elf main.c minimax.c ticTacToeControl.c ticTacToeDisplay.c testBoards.c)
target_link_libraries(lab5.elf ${330_LIBS} intervalTimer buttons_switches)
set_target_properties(lab5.elf PROPERTIES LINKER_LANGUAGE CXX)