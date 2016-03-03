#ifndef _HEAD_SERVICE_SCALE_BASED_DATA_H_
#define _HEAD_SERVICE_SCALE_BASED_DATA_H_
#include <stdio.h>
enum {
	  WEIGHT_TEST_OK  = 0,
	  WEIGHT_TEST_FAIL = 1,
	  SCALE_COMMUNICATION_FAIL = 2,
      SCALE_PROBLEM =3,
	  };
class HSWScaledBaseData
{
	private:
	std::vector<bool> m_activeThermistors;
	// save start heating time
	unsigned m_StartHeatingTime;
	// frame is allready printed or not
	bool m_isFramePrinted;
	std::vector<int>m_missingNozzles;
	std::vector<float>m_measurements;
	std::vector<float>m_measurements_v1;
	std::vector<float>m_measurements_v2;
	std::vector<float>m_gain_v1;
	std::vector<float>m_gain_v2;
    std::vector<float>m_measurementsForMaxVoltage;
	bool m_IsModelReplacement;
	int m_currentOperationMode;
	int m_currentQualityMode;
	bool m_scaleOnTray ;
	bool m_headReplacement;
	QString m_m3ModelName;
 public:
	//Public Methods:
	bool m_lastMRW;
	bool m_stressTestDone;
	HSWScaledBaseData(void)
	{
	 m_IsModelReplacement = false;
	 m_currentOperationMode = 0;
	 m_currentQualityMode = 0;
	 m_activeThermistors.clear();
	 m_StartHeatingTime = 0;
	 m_isFramePrinted = false;
	 m_measurements.clear();
	 m_measurementsForMaxVoltage.clear();
	 m_measurements_v2.clear();
	 m_measurements_v1.clear();
	 m_gain_v1.clear();
	 m_gain_v2.clear();
	 m_lastMRW = false;
	 m_stressTestDone = false;
	 m_scaleOnTray = false;
	 m_headReplacement = false;
	}
	virtual ~HSWScaledBaseData(void)
	{

	}

	int GetCurrentOperationMode()
	{
		return m_currentOperationMode;
	}
	void SetCurrentOperationMode(int operationMode)
	{
	  m_currentOperationMode = operationMode;
	}
	int GetCurrentQualityMode()
	{
		return m_currentQualityMode;
	}
	void SetCurrentQualityMode(int currentQualityMode)
	{
	   m_currentQualityMode = currentQualityMode;
	}
	bool IsModelReplacement()
	{
	  return m_IsModelReplacement;
	}
	void SetModelReplacement(bool modelReplacement)
	{
	   m_IsModelReplacement = modelReplacement;
	}
	std::vector<int> GetMissingNozzles()
	{
		return m_missingNozzles;
	}
	int GetMissingNozzlesPerHead(int headID)
	{
		return m_missingNozzles[headID];
    }
	void ClearMissingNozzles()
	{
		if(!m_missingNozzles.empty())
			m_missingNozzles.clear();
	}
	void SetMissingNozzles(std::vector<int>missingNozzles)
	{
	if(!m_missingNozzles.empty())
	  m_missingNozzles.clear();
	m_missingNozzles = missingNozzles;
	}
	bool IsFramePrinted()
	{
		return  m_isFramePrinted;
	}
	void setFramePrinted(bool framePrinted)
	{
	  m_isFramePrinted = framePrinted;
	}
	unsigned GetStartHeatingTime()
	{
		return m_StartHeatingTime;
	}
	void SetStartHeatingTime(unsigned startHeatingTime)
	{
	  m_StartHeatingTime = startHeatingTime;
	}
	std::vector<bool> GetActiveThermistors()
	{
		return m_activeThermistors;
	}
	bool GetActiveThermistor(int index)
	{
		return m_activeThermistors[index];
	}
	void SetActiveThermistors(std::vector<bool> activeThermistors)
	{
		m_activeThermistors.clear();
		m_activeThermistors = activeThermistors;
	}
	void ClearActiveThermistors()
	{
       m_activeThermistors.clear();
	}
	void AddActiveThermistor(bool active)
	{
       m_activeThermistors.push_back(active);
	}
	void ClearMeasurements()
	{
		m_measurements.clear();
		m_measurementsForMaxVoltage.clear();
		m_measurements_v2.clear();
		m_measurements_v1.clear();
		m_gain_v1.clear();
		m_gain_v2.clear();
	}
	void AddMeasurementForV1(float measurement)
	{
		m_measurements_v1.push_back(measurement);
	}
	void AddMeasurementForV2(float measurement)
	{
		m_measurements_v2.push_back(measurement);
	}
	void AddGainV1(float gain)
	{
		m_gain_v1.push_back(gain);
	}
	void AddGainV2(float gain)
	{
		m_gain_v2.push_back(gain);
	}
	void ChangeGain(float gain,int headID)
	{
		m_gain_v2[headID] = gain;
	}
	float GetGainV1(int headID)
	{
		return  m_gain_v1[headID];
	}
	float GetGainV2(int headID)
	{
		return  m_gain_v2[headID];
	}
	float GetMeasurementForV1(int headID)
	{
		return m_measurements_v1[headID];
	}
	 float GetMeasurementForV2(int headID)
	{
		return m_measurements_v2[headID];
	}
	void AddMeasurementForMaxVoltage(float measurement)
	{
		m_measurementsForMaxVoltage.push_back(measurement);
	}
	float GetMeasurementForMaxVoltage(int headID)
	{
		return m_measurementsForMaxVoltage[headID];
	}
	void AddMeasurement(float measurement)
	{
		m_measurements.push_back(measurement);
	}
	void ChangeMeasurement(float measurement,int headID)
	{
		m_measurements[headID] = measurement;
	}
	float GetMeasurement(int headID)
	{
	  return m_measurements[headID];
	}
	void SetScaleOnTray(bool status)
	{
		m_scaleOnTray = status;
	}
	bool IsScaleOnTray()
	{
		return m_scaleOnTray;
	}
	void SetHeadReplacement(bool status)
	{
		m_headReplacement = status;
	}
	bool IsHeadReplacement()
	{
		return  m_headReplacement;
	}
	void SetM3ModelName(QString m3ModelName)
	{
		m_m3ModelName = m3ModelName;
	}
	QString GetM3ModelName()
	{
		return  m_m3ModelName;
    }
};
class HSWCalibrationData
{
   private:
	 std::vector<int> m_HeadPotentiometerValues;
	 float m_LayerHeightDPI_um;
	 std::vector<float> m_HeadsVoltagesModel;
	 std::vector<float> m_HeadsVoltagesSupport;
   public:
   HSWCalibrationData(void)
   {
	  m_HeadPotentiometerValues.clear();
	  m_LayerHeightDPI_um = 0;
	  m_HeadsVoltagesModel.clear();
	  m_HeadsVoltagesSupport.clear();
   }
   virtual ~HSWCalibrationData(void)
   {

   }
   void ClearData()
   {
	  if(!m_HeadPotentiometerValues.empty())
	  m_HeadPotentiometerValues.clear();
	  if(!m_HeadsVoltagesModel.empty())
		m_HeadsVoltagesModel.clear();
	  if(!m_HeadsVoltagesSupport.empty())
		m_HeadsVoltagesSupport.clear();
   }
   int GetHeadPotentiometerValues(int head)
   {
	if(!m_HeadPotentiometerValues.empty())
	   return m_HeadPotentiometerValues[head];
	return 0;
   }
   void SetHeadPotentiometerValues(int headPotentiometerValues)
   {
	 m_HeadPotentiometerValues.push_back(headPotentiometerValues);
   }
   float GetLayerHeightDPI_um()
   {
	   return  m_LayerHeightDPI_um;
   }
   void SetLayerHeightDPI_um(float layerHeightDPI_um)
   {
	   m_LayerHeightDPI_um = layerHeightDPI_um;
   }
   float GetHeadsVoltagesModel(int head)
   {
   if(!m_HeadsVoltagesModel.empty())
	   return m_HeadsVoltagesModel[head];
   return 0;
   }
   void SetHeadsVoltagesModel(float headsVoltagesModel)
   {
	m_HeadsVoltagesModel.push_back(headsVoltagesModel);
   }
   float GetHeadsVoltagesSupport(int head)
   {
   if(!m_HeadsVoltagesSupport.empty())
	   return m_HeadsVoltagesSupport[head];
	return 0;
   }
   void SetHeadsVoltagesSupport(float headsVoltagesSupport)
   {
	m_HeadsVoltagesSupport.push_back(headsVoltagesSupport);
   }

};

#endif
