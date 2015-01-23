/**
 * \file cameraproperty.h
 * \author Virgile Wozny
 */
#ifndef CAMERAPROPERTY_H
#define CAMERAPROPERTY_H

#include <string>
#include <cmath>
#include <QString>

namespace CameraManager {

	enum PropertyType {
        BRIGHTNESS,
        HUE,
        GAIN,
        EXPOSURE,
        GAMMA,
        SHUTTER,
        FRAMERATE,
        CROPX,
        CROPY,
        CROP,
        PANX,
        PANY,
        PAN,
        AUTOTRIGGER
    };

	/**
	 * CameraProperty
	 * \brief memorize one property (value and auto mode) that will be exchanged between the manager end the cameras
	 */
    class CameraProperty
	{
	public:
        /**
         * @brief Contructor
         * @param type type of the property
         * @param min minimum value for that property
         * @param max maximum value for that property
         * @param decimals number of decimal for that property (used for the slider)
         * @param canAuto true if that property can be set to auto (will desactivate the slider)
         */
		CameraProperty(PropertyType type, float min, float max, int decimals = 0, bool canAuto = false);
    /**
    * @brief Contructor
    * @param type type of the property
    * @param min minimum value for that property
    * @param max maximum value for that property
    * @param decimals number of decimal for that property (used for the slider)
    * @param canAuto true if that property can be set to auto (will desactivate the slider)
    * @param val property value
    */
    CameraProperty(PropertyType type, float min, float max, int decimals, bool canAuto, float value);  // gs 2015-01-20, for å sette startverdi på cameraproperties
		PropertyType getType(){ return type; }
		void setAuto(bool a){ isAuto = a; }
		bool getAuto(){ return isAuto; }
		bool getCanAuto(){ return canAuto; }
		void setValue(float value){ val = value; }
		float getValue(){ return val; }
		float getDecimals(){ return decimals; }

		void setValueFromSlider(int value){ val = value/pow(10, (double) decimals); }
		int getValueToSlider(){ return val*pow(10, (double) decimals); }
		int getMinToSlider(){ return min*pow(10, (double) decimals); }
		int getMaxToSLider(){ return max*pow(10, (double) decimals); }
		QString formatValue(){ return QString::number(val, 'f', decimals); }

		std::string getName();
	private:
		PropertyType type;
        float min;
        float max;
		int decimals;
		bool canAuto;

		float val;
		bool isAuto;
	};
}

#endif // CAMERAPROPERTY_H
