/*!
 * \file	davang_uart.h
 * \details UART abstraction class declaration for ESP-IDF.
 * \author	Davang
 * \version	1.0.0.a
 * \date	2023/05/20
 * \copyright MIT License
 */

#ifndef ESP_DAVANG_COMPONENTS__GPIO_H
#define ESP_DAVANG_COMPONENTS__GPIO_H

/* C/C++ includes */
#include <utility>
#include <cstdint>
#include <errno.h>

/* System specific libaries */
#include "driver/gpio.h"
#include "esp_err.h"

/* Davang includes */

/* 3rd party includes */


/*!< Specific davang namespace for uart data types related. */
namespace  dvng::gpio
{

/* data types */
/*!< Specific davang namespace for uart data types related. */
using pin = uint32_t;
using isr = gpio_isr_t;

/* enumerators */

enum class MODE : uint32_t
{
	INPUT = GPIO_MODE_INPUT ,
	OUTPUT = GPIO_MODE_OUTPUT ,
	OUTPUT_OPEN_DRAIN = GPIO_MODE_OUTPUT_OD ,
};


enum class PULL_UP : uint32_t
{
	DISABLE = GPIO_PULLUP_DISABLE ,
	ENABLE = GPIO_PULLUP_ENABLE,
};

enum class PULL_DOWN : uint32_t
{
	DISABLE = GPIO_PULLDOWN_DISABLE ,
	ENABLE = GPIO_PULLDOWN_ENABLE ,
};

enum class INTERRUPT : uint32_t
{
	NONE = GPIO_INTR_DISABLE  ,
	RAISE = GPIO_INTR_POSEDGE ,
	FALL = GPIO_INTR_NEGEDGE ,
	EDGE = GPIO_INTR_ANYEDGE ,
	LOW = GPIO_INTR_LOW_LEVEL ,
	HIGH = GPIO_INTR_HIGH_LEVEL ,
};

enum class LEVEL : uint32_t
{
	LOW = 0  ,
	HIGH = 1 ,
};


/* constants */

/*!< Specific davang namespace for uart data types related. */
constexpr pin MAX_PIN = GPIO_PIN_COUNT;


}



/*!< Generic davang namespace */
namespace dvng
{
class ci_gpio
{
	virtual int init( ) = 0;
	virtual int set_level( const gpio::LEVEL & t_level ) = 0;
	virtual gpio::LEVEL get_level( ) = 0;
	virtual int set_high( ) = 0;
	virtual int toggle( ) = 0;
	virtual int set_low( ) = 0;
	virtual int register_isr( gpio::isr t_isr, void * t_arguments ) = 0;
};

template < gpio::pin T_PIN, gpio::MODE T_MODE, gpio::PULL_UP T_PULLUP, gpio::PULL_DOWN T_PULLDOWN, gpio::INTERRUPT T_INTERRUPT = gpio::INTERRUPT::NONE >
class c_gpio : ci_gpio
{
/* static asserts */
	static_assert( gpio::MAX_PIN > T_PIN, " PIN should be less than the num of gpios in the device" );	
/* data members */
public:
protected:
private:
	const gpio_config_t m_gpio_config;
	gpio::LEVEL m_level;
/* static data members */
public:
protected:
private:

/* constructors and destructor */
public:
	constexpr c_gpio( ) : c_gpio( gpio::LEVEL::LOW )
	{
		// do nothing in the body.
	};

	constexpr c_gpio( const gpio::LEVEL t_level ) :  m_gpio_config ( /* pin_bit_mask */ 1ULL << T_PIN,
			/* mode */ static_cast< gpio_mode_t >( T_MODE ),
			/* pull_up_en */ static_cast< gpio_pullup_t  >( T_PULLUP ),
			/* pull_down_en */ static_cast< gpio_pulldown_t  >( T_PULLDOWN ),
			/* intr_type */ static_cast< gpio_int_type_t  >( T_INTERRUPT ) ) ,
		m_level ( t_level )
	{
		// do nothing in the body.
	}
	
protected:
private:


/* methods */
public:	
	
	[[nodiscard("Always ensure valid pin initialization")]]
	virtual int init( ) override
	{
		return  gpio_config( &m_gpio_config );
	}

	[[nodiscard("Why get the level of a pin if not using it?")]]
	virtual gpio::LEVEL get_level( ) override
	{
		return  static_cast< gpio::LEVEL >( gpio_get_level( static_cast< gpio_num_t >( T_PIN ) ) );
	}

	virtual int set_level( const gpio::LEVEL & t_level ) override
	{
		if constexpr( gpio::MODE::INPUT == T_MODE )
		{
			return ESP_ERR_NOT_SUPPORTED;
		}
		else
		{
			m_level = t_level;
			return gpio_set_level( static_cast< gpio_num_t >( T_PIN ), std::to_underlying( m_level ) );			
		}
	}
	
	virtual int set_high( ) override
	{
		return set_level( gpio::LEVEL::HIGH );
	}
	
	virtual int set_low( ) override
	{
		return set_level( gpio::LEVEL::LOW );
	}
	
	virtual int toggle( ) override
	{
		if( gpio::LEVEL::LOW == m_level )
		{
			return set_high( );
		}
		else
		{
			return set_low( );	
		}
	}
	
	[[nodiscard("Always ensure correct isr registration ")]]
	virtual int register_isr( gpio::isr t_isr, void * t_arguments ) override
	{
		if constexpr( gpio::MODE::INPUT == T_MODE )
		{
			return ESP_ERR_NOT_SUPPORTED;
		}
		else
		{
			return gpio_isr_handler_add( static_cast< gpio_num_t >( T_PIN ), t_isr, t_arguments );
		}
	}

protected:
private:

/* static methodss */
public:
protected:
private:

};

namespace gpio
{

template < gpio::pin T_PIN, gpio::INTERRUPT T_INTERRUPT = gpio::INTERRUPT::NONE >
using c_input = c_gpio< T_PIN, gpio::MODE::INPUT, gpio::PULL_UP::DISABLE, gpio::PULL_DOWN::DISABLE, T_INTERRUPT >;

template < gpio::pin T_PIN, gpio::INTERRUPT T_INTERRUPT = gpio::INTERRUPT::NONE >
using c_pull_up_input = c_gpio< T_PIN, gpio::MODE::INPUT, gpio::PULL_UP::ENABLE, gpio::PULL_DOWN::DISABLE, T_INTERRUPT 	>;

template < gpio::pin T_PIN, gpio::INTERRUPT T_INTERRUPT = gpio::INTERRUPT::NONE >
using c_pull_down_input = c_gpio< T_PIN, gpio::MODE::INPUT, gpio::PULL_UP::DISABLE, gpio::PULL_DOWN::ENABLE, T_INTERRUPT >;

template < gpio::pin T_PIN, gpio::INTERRUPT T_INTERRUPT = gpio::INTERRUPT::NONE >
using c_output = c_gpio< T_PIN, gpio::MODE::OUTPUT, gpio::PULL_UP::DISABLE, gpio::PULL_DOWN::DISABLE, T_INTERRUPT >;

template < gpio::pin T_PIN, gpio::PULL_UP T_PULLUP, gpio::PULL_DOWN T_PULLDOWN, gpio::INTERRUPT T_INTERRUPT = gpio::INTERRUPT::NONE >
using c_open_drain_output = c_gpio< T_PIN, gpio::MODE::OUTPUT_OPEN_DRAIN, T_PULLUP, T_PULLDOWN, T_INTERRUPT >;

template < gpio::pin T_PIN, gpio::INTERRUPT T_INTERRUPT = gpio::INTERRUPT::NONE >
using c_pull_up_output = c_gpio< T_PIN, gpio::MODE::OUTPUT, gpio::PULL_UP::ENABLE, gpio::PULL_DOWN::DISABLE, T_INTERRUPT >;

template < gpio::pin T_PIN, gpio::INTERRUPT T_INTERRUPT = gpio::INTERRUPT::NONE >
using c_pull_down_output = c_gpio< T_PIN, gpio::MODE::OUTPUT, gpio::PULL_UP::DISABLE, gpio::PULL_DOWN::ENABLE, T_INTERRUPT >;

}

}

#endif /*  ESP_DAVANG_COMPONENTS__GPIO_H */