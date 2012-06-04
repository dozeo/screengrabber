#ifndef LINUX

#include <gtest/gtest.h>
#include <grabber/WindowInfo.h>
#include <grabber/ProcessInfo.h>

class EnumerateWindowsTest : public testing::Test {
protected:
};

TEST_F (EnumerateWindowsTest, getWindows) {
	typedef std::vector<dz::WindowInfo> WindowVec;
	WindowVec windows;
	int result = dz::WindowInfo::populate (&windows);
	EXPECT_EQ (dz::Grabber::GE_OK, result) << "Should get windows list";

	EXPECT_TRUE (windows.size() > 0) << "Should have at least one window";

	int validProcess = 0;
	int validWin = 0;
	for (WindowVec::const_iterator i = windows.begin(); i != windows.end(); i++) {
		const dz::WindowInfo & info (*i);
		std::cout << info.id << " from " << info.pid << " " << info.title << " on " << info.area << std::endl;
		const dz::ProcessInfo pinfo = dz::ProcessInfo::about (info.pid);
		if (pinfo.valid() && !pinfo.exec.empty())
			validProcess++;
		dz::WindowInfo second= dz::WindowInfo::about (info.id);
		if (second.valid () && second.id == info.id && second.pid == info.pid && second.title == info.title && second.area == info.area){
			validWin++;
		}
	}
	EXPECT_TRUE (validProcess > 0) << "Should get process info from at least one process directly";
	EXPECT_TRUE (validWin > 0) << "Should get window info for at least one window directly";
}

TEST_F (EnumerateWindowsTest, getProcesses) {
	typedef std::vector<dz::ProcessInfo> ProcessVec;
	ProcessVec processes;
	int result = dz::ProcessInfo::populate (&processes);
	EXPECT_EQ (dz::Grabber::GE_OK, result) << "Should get process list" << std::endl;
	EXPECT_TRUE (processes.size() > 0) << "Should have at least one process?!" << std::endl;
	for (ProcessVec::const_iterator i = processes.begin(); i != processes.end(); i++) {
		const dz::ProcessInfo & info (*i);
		std::cout << "Process " << info.pid << " exec: " << info.exec << std::endl;
	}
}
#endif
