#include <gtest/gtest.h>
#include "CBackup.h"

TEST(BackupTest, BasicBackup){
    CBackup backup;
    bool result = backup.doBackup("source", "dest");
    EXPECT_TRUE(result);
}