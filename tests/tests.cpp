#include <CppUnitLite/TestHarness.h>

int GetTotal() {
	return 0;
}

int main() {
	TestResult tr;
	TestRegistry::runAllTests(tr);
	
	return 0;
}

TEST(Sale, totalNewSale) {
	CHECK(0 == GetTotal());
}