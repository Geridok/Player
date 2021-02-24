#pragma once
#include <vector>

class CSignature
{
	friend class CSignatureCalculator;
public:
	// Use to divide one signature from video capture between different samples.
	// Changed algorithm in situation when current sample's signature and current signature from video capture are mismatched uses signatures of previous 
	// frames from video capture for comparison. These signatures can be used by several samples so its must be deleted only when no ones use them.
	// That's why AddRef/Release mechanism is used.
	CSignature();
	void addRef();	// use one more time - increase reference counter (as std::shared_ptr)
	void release(); // decrease reference counter and remove the object on last decrease

	// main function - calculate difference between two signatures
	double difference(const CSignature &signature);

	// save/load signature data to/from string
	void loadFromString(const CString &sigStr);
	void saveToString(CString &sigStr);
	size_t getSize(void) { return m_signature.size(); }
	~CSignature();
protected:
	bool compare(const CSignature &signature);
	bool compareShots(const CSignature &signature);

	void insertNumber(double num, double fourierCoeffMin);

	double getThreshold() const { return m_threshold; }
	void setThreshold(double val) { m_threshold = val; }
	double getThresholdBetweenShots() const { return m_thresholdBetweenShots; }
	void setThresholdBetweenShots(double val) { m_thresholdBetweenShots = val; }
	const double getDefaultThreshold() const { return m_cDefaultThreshold; }
	const double getDefaultThresholdBetweenShots() const { return m_cDefaultThresholdBetweenShots; }

	void normalize(void);

private:
	

	std::vector<double> m_signature;
	double m_threshold;
	double m_thresholdBetweenShots;
	double m_sum;
	CComAutoCriticalSection m_critSec;

	// Reference counter. When it becomes "0" signature would be deleted.
	long m_refCounter;

	// Constant
	const double m_cDefaultThreshold;
	const double m_cDefaultThresholdBetweenShots;
};