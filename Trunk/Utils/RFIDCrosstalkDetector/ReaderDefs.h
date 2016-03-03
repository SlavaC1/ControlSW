#ifndef ReaderDefsH
#define ReaderDefsH

#include <map>

const int NUM_OF_CONTAINERS = 4;
const int NUM_OF_COM_PORTS  = 10;

typedef std::map<__int64, int> TResultsTable; // <Tag ID, Counts>

const QString ContainerNames[NUM_OF_CONTAINERS] =
{
	"Model 1", "Model 2", "Support 1", "Support 2"
};

enum TContainer
{
	MODEL_1,
	MODEL_2,
	SUPPORT_1,
	SUPPORT_2
};

const QString COMPorts[NUM_OF_COM_PORTS] =
{
	"COM 1",
	"COM 2",
	"COM 3",
	"COM 4",
	"COM 5",
	"COM 6",
	"COM 7",
	"COM 8",
	"COM 9",
	"COM 10"
};

struct TTestResults
{
	TTestResults()
	{
        Passed = true;

		for(int i = 0; i < NUM_OF_CONTAINERS; i++)
			Percentage[i] = 0;
    }

	TResultsTable ResultsPerTag[NUM_OF_CONTAINERS];

	int Percentage[NUM_OF_CONTAINERS];
	bool Passed;
};

struct TTestData
{
	TTestData()
	{
		TestingCycles     = 0;
		PassFailCriteria  = 0;
		SelectedCOM       = 0;
		SelectedContainer = MODEL_1;
	}

	int TestingCycles;
	int PassFailCriteria;
	int SelectedCOM;
	TContainer SelectedContainer;
	TTestResults TestResults;
};


#endif 