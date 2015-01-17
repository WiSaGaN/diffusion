// Created on April 20, 2014 by Lu, Wangshan.
// Unit test for file component.
#include <diffusion/factory.hpp>
#include <gtest/gtest.h>
TEST(WriterContructor, FileExists) {
}
TEST(WriterContructor, OpenFileError) {
}
TEST(WriterWrite, WriteError) {
}
TEST(ReaderConstructor, FileNotExists) {
}
TEST(ReaderConstructor, NotDFSNFile) {
}
TEST(ReaderCanRead, StopAtEOF) {
}
TEST(ReaderRead, DFSNFileIncomplete) {
}
TEST(ReaderRead, CorruptedMessageHeader) {
}
int main(int argc, char * argv[]) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}