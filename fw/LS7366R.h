// Copyright 2018-2022 Guru Subramani, guru.subramani.g.s@gmail.com.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#pragma once

#include "mbed.h"

#include "hal/spi_api.h"

#include "fw/ccm.h"
#include "fw/moteus_hw.h"
#include "fw/stm32_spi.h"





namespace moteus {

class LS7366R {
 public:
  using Options = Stm32Spi::Options;

  LS7366R(const Options& options)
    : spi_(options) {
    
  }

  uint16_t Sample() MOTEUS_CCM_ATTRIBUTE {

    if(!initialized_)
    {
        Init();
        initialized_ = true;
    }

    return (spi_.write(0xffff) & 0x3fff) << 2;
  }

  void StartSample() MOTEUS_CCM_ATTRIBUTE {

    if(!initialized_)
    {
        Init();
        initialized_ = true;
    }

    return spi_.write_opcode(READ_CNTR);
  }

  uint16_t FinishSample() MOTEUS_CCM_ATTRIBUTE {

    return (spi_.read_write_two_bytes(0xffff));
  }
  
 private:
  Stm32Spi spi_;


  void Init() MOTEUS_CCM_ATTRIBUTE {
    
    spi_.write( static_cast<uint8_t>(WRITE_MDR0), static_cast<uint8_t>(QUADRX4 | FREE_RUN | DISABLE_INDX | FILTER_1));        
    spi_.write( static_cast<uint8_t>(WRITE_MDR1), static_cast<uint8_t>(EN_CNTR | BYTE_2 | RANGE_LIMIT));

    // setting the range of the encoder
    spi_.write( static_cast<uint8_t>(WRITE_DTR), static_cast<uint16_t>(cpr_));

  }
  
  bool initialized_ = false;
  const uint16_t cpr_ = 10000;


  /* Constant variable declarations */
  //LS7366R 4 channel quadrature encoder

  //Count modes 
  const uint8_t NQUAD = 0x00; //non-quadrature mode 
  const uint8_t QUADRX1 = 0x01; //X1 quadrature mode 
  const uint8_t QUADRX2 = 0x02; //X2 quadrature mode 
  const uint8_t QUADRX4 = 0x03; //X4 quadrature mode 
  //Running modes 
  const uint8_t FREE_RUN = 0x00; 
  const uint8_t SINGE_CYCLE = 0x04; 
  const uint8_t RANGE_LIMIT = 0x08; 
  const uint8_t MODULO_N = 0x0C; 
  //Index modes 
  const uint8_t DISABLE_INDX = 0x00; //index_disabled 
  const uint8_t INDX_LOADC = 0x10; //index_load_CNTR 
  const uint8_t INDX_RESETC = 0x20; //index_rest_CNTR 
  const uint8_t INDX_LOADO = 0x30; //index_load_OL 
  const uint8_t ASYNCH_INDX = 0x00; //asynchronous index 
  const uint8_t SYNCH_INDX = 0x80; //synchronous index 
  //Clock filter modes 
  const uint8_t FILTER_1 = 0x00; //filter clock frequncy division factor 1 
  const uint8_t FILTER_2 = 0x80; //filter clock frequncy division factor 2 
  /* **MDR1 configuration data; any of these*** 
  ***data segments can be ORed together***/ 
  //Flag modes 
  const uint8_t NO_FLAGS = 0x00; //all flags disabled 
  const uint8_t IDX_FLAG = 0x10; //IDX flag 
  const uint8_t CMP_FLAG = 0x20; //CMP flag 
  const uint8_t BW_FLAG = 0x40; //BW flag 
  const uint8_t CY_FLAG = 0x80; //CY flag 
  //1 to 4 bytes data-width 
  const uint8_t BYTE_4 = 0x00; //four byte mode 
  const uint8_t BYTE_3 = 0x01; //three byte mode 
  const uint8_t BYTE_2 = 0x02; //two byte mode 
  const uint8_t BYTE_1 = 0x03; //one byte mode 
  //Enable/disable counter 
  const uint8_t EN_CNTR = 0x00; //counting enabled 
  const uint8_t DIS_CNTR = 0x04; //counting disabled 

  /* LS7366R op-code list */ 
  const uint8_t CLR_MDR0 = 0x08; 
  const uint8_t CLR_MDR1 = 0x10; 
  const uint8_t CLR_CNTR = 0x20; 
  const uint8_t CLR_STR = 0x30; 
  const uint8_t READ_MDR0 = 0x48; 
  const uint8_t READ_MDR1 = 0x50;

  const uint8_t READ_CNTR = 0x60; 
  const uint8_t READ_OTR = 0x68; 
  const uint8_t READ_STR = 0x70; 
  const uint8_t WRITE_MDR1 = 0x90; 
  const uint8_t WRITE_MDR0 = 0x88; 
  const uint8_t WRITE_DTR = 0x98; 
  const uint8_t LOAD_CNTR = 0xE0; 
  const uint8_t LOAD_OTR = 0xE4; 
  
};

}
    
/* TODOs: 
* DONE; convert #defines to const variables
* create state machine to init the LS7366R in LS7366R.h 
* update aux_port.h 
  * setup the bits for the SPI interface for LS7366R in HandleConfigUpdate
  * add LS7366R_options_ to aux_port.h similar to as5047_options_
  * 
* update aux_common.h
  * add kLS7366R to Spi::Config::Mode (enum)
  
* update motor_position.h
  * add a SourceConfig enum type kSpiQuadrature
  * in ISR_UpdateSources()
    * add a case that is a hybrid to SourceConfig::kSpi and SourceConfig::KQuadrature

|< WRITE_DTR >| 16 bit number |


*/