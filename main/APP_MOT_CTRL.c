#include "LL_ISR.h"
#include "LL_TIMERS.h"
#include "APP_MOT_CTRL.h"
#include "APP_HALL_SENS.h"

#define NMBR_OF_LUT_STEPS			514u
#define NMBR_OF_PHASES				3u
#define OFFSET_PHASE_A				0u
#define OFFSET_PHASE_B				21845u
#define OFFSET_PHASE_C				43690u
#define IDX_PHASE_A				0u
#define IDX_PHASE_B				1u
#define IDX_PHASE_C				2u
#define RATIO_SECTOR_TO_RAW			10922

const uint16_t SvmLut[NMBR_OF_LUT_STEPS] =
{
	1550u,1550u, 1549u, 1549u, 1549u, 1548u, 1547u, 1546u,1545u,
	1544u,1543u,1542u,1540u,1539u,1537u,1536u,1534u,1532u,1530u,
	1528u,1525u,1523u,1520u,1518u,1515u,1512u,1509u,1506u,1503u,
	1500u,1496u,1493u,1489u,1486u,1482u,1478u,1474u,1470u,1466u,
	1462u,1457u,1453u,1448u,1443u,1439u,1434u,1429u,1424u,1419u,
	1413u,1408u,1403u,1397u,1391u,1386u,1380u,1374u,1368u,1362u,
	1356u,1350u,1343u,1337u,1331u,1324u,1317u,1311u,1304u,1297u,
	1290u,1283u,1276u,1269u,1262u,1254u,1247u,1240u,1232u,1225u,
	1217u,1209u,1202u,1194u,1186u,1178u,1170u,1162u,1154u,1146u,
	1138u,1129u,1121u,1113u,1104u,1096u,1087u,1079u,1070u,1062u,
	1053u,1044u,1036u,1027u,1018u,1009u,1001u,992u,983u,974u,
	965u,956u,947u,938u,929u,920u,911u,901u,892u,883u,
	874u,865u,856u,847u,837u,828u,819u,810u,801u,791u,
	782u,773u,764u,755u,745u,736u,727u,718u,709u,700u,
	691u,682u,672u,663u,654u,645u,636u,627u,618u,610u,
	601u,592u,583u,574u,565u,557u,548u,539u,531u,522u,
	514u,505u,497u,488u,480u,472u,463u,455u,447u,439u,
	431u,423u,415u,407u,399u,392u,384u,376u,369u,361u,
	354u,347u,339u,332u,325u,318u,311u,304u,297u,290u,
	283u,277u,270u,264u,257u,251u,245u,239u,233u,227u,
	221u,215u,209u,204u,198u,193u,187u,182u,177u,172u,
	167u,162u,157u,153u,148u,143u,139u,135u,131u,126u,
	122u,119u,115u,111u,107u,104u,101u,97u,94u,91u,
	88u,85u,83u,80u,77u,75u,73u,71u,68u,66u,
	65u,63u,61u,60u,58u,57u,56u,55u,54u,53u,
	52u,51u,51u,51u,50u,50u,50u,50u,50u,50u,
	51u,51u,52u,53u,53u,54u,55u,57u,58u,59u,
	61u,62u,64u,66u,68u,70u,72u,74u,77u,79u,
	82u,85u,87u,90u,93u,97u,100u,103u,107u,110u,
	114u,118u,121u,125u,129u,134u,138u,142u,147u,151u,
	156u,161u,166u,171u,176u,181u,186u,191u,197u,202u,
	208u,214u,219u,225u,231u,237u,243u,250u,256u,262u,
	269u,275u,282u,288u,295u,302u,309u,316u,323u,330u,
	337u,345u,352u,359u,367u,374u,382u,390u,397u,405u,
	413u,421u,429u,437u,445u,453u,461u,470u,478u,486u,
	495u,503u,511u,520u,529u,537u,546u,554u,563u,572u,
	581u,589u,598u,607u,616u,625u,634u,643u,652u,661u,
	670u,679u,688u,697u,706u,716u,725u,734u,743u,752u,
	761u,771u,780u,789u,798u,807u,817u,826u,835u,844u,
	853u,863u,872u,881u,890u,899u,908u,917u,926u,935u,
	944u,954u,962u,971u,980u,989u,998u,1007u,1016u,1025u,
	1033u,1042u,1051u,1060u,1068u,1077u,1085u,1094u,1102u,1111u,
	1119u,1127u,1135u,1144u,1152u,1160u,1168u,1176u,1184u,1192u,
	1200u,1207u,1215u,1223u,1230u,1238u,1245u,1253u,1260u,1267u,
	1274u,1281u,1288u,1295u,1302u,1309u,1316u,1322u,1329u,1335u,
	1342u,1348u,1354u,1360u,1367u,1373u,1378u,1384u,1390u,1396u,
	1401u,1407u,1412u,1417u,1422u,1427u,1432u,1437u,1442u,1447u,
	1451u,1456u,1460u,1465u,1469u,1473u,1477u,1481u,1485u,1488u,
	1492u,1496u,1499u,1502u,1505u,1508u,1511u,1514u,1517u,1520u,
	1522u,1525u,1527u,1529u,1531u,1533u,1535u,1537u,1539u,1540u,
	1542u,1543u,1544u,1545u,1546u,1547u,1548u,1548u,1549u,1549u,
	1550u,1550u,1550u
};

const uint16_t PhAngleOffset[NMBR_OF_PHASES] = { OFFSET_PHASE_A, OFFSET_PHASE_B, OFFSET_PHASE_C };

typedef struct Svm_t
{
	uint16_t	Angle;
	uint16_t	Amp;
	uint16_t	Sector;
} Svm_t;

static Svm_t Svm;

typedef struct Ramp_t
{
 	uint16_t	CurrVal;
	uint16_t	Step;
	uint16_t	EndVal;
}Ramp_t;

static Ramp_t		SpeedRamp;
static Ramp_t		AmplRamp;

static uint32_t		CurrTicks = 0u;

/**
  * @brief This function generate sine wave
  * @param none
  * @retval none
  */
static void DoRamps( Ramp_t * pRamp )
{
	if ( pRamp->EndVal > pRamp->CurrVal )
		pRamp->CurrVal += pRamp->Step;
}

/**
  * @brief This function generate sine wave
  * @param none
  * @retval none
  */
static void GenSvm( Svm_t *pSvm )
{
	uint16_t		phADutyCycle = 0u;
	uint16_t		phBDutyCycle = 0u;
	uint16_t		phCDutyCycle = 0u;
	uint16_t		Offset = 0u;
	uint16_t		idx = 0u;

	Offset = ( Svm.Angle + PhAngleOffset[IDX_PHASE_A] );
	idx = ( Offset >> 7u );
	phADutyCycle = ( ( SvmLut[idx] * Svm.Amp ) / 100u );
	
	Offset = ( Svm.Angle + PhAngleOffset[IDX_PHASE_B] );
	idx = ( Offset >> 7u );
	phBDutyCycle = ( ( SvmLut[idx] * Svm.Amp ) / 100u );
	
	Offset = ( Svm.Angle + PhAngleOffset[IDX_PHASE_C] );
	idx = ( Offset >> 7u );
	phCDutyCycle = ( ( SvmLut[idx] * Svm.Amp ) / 100u );

	LL_TIMERS_SetDutyPhA( phADutyCycle );
	LL_TIMERS_SetDutyPhB( phBDutyCycle );
	LL_TIMERS_SetDutyPhC( phCDutyCycle );
}

/**
  * @brief This function initialize initialize local variables
  * @param none
  * @retval none
  */
void APP_MOT_CTRL_Init( void )
{
	CurrTicks = LL_ISR_SysTicks;

	Svm.Angle		= 0u;
	Svm.Amp			= 0u;
	Svm.Sector		= 0u;

	SpeedRamp.CurrVal	= 0u;
	SpeedRamp.Step		= 1u;
	SpeedRamp.EndVal	= 1000u;

	AmplRamp.CurrVal	= 0u;
	AmplRamp.Step		= 1u;
	AmplRamp.EndVal		= 15u;
}

/**
  * @brief This function invoke all functionalities
  * @param none
  * @retval none
  */
void APP_MOT_CTRL_Task( void )
{
	static uint16_t		rampPresc = 0u;
	static uint16_t		CalcAngle = 0u;
	static uint8_t		State = 0u;

	switch( State )
	{
		case 0u:
			if ( 1000u == ( LL_TIMERS_TIMEDIFF( LL_ISR_SysTicks, CurrTicks ) ) )
			{
				Svm.Amp		= 0u;
				Svm.Angle	= 0u;
				CurrTicks	= LL_ISR_SysTicks;
				State		= 1u;
				printf( "Rotor aligned\n" );
			}
			else
			{
				Svm.Angle	= 54000u;
				Svm.Amp		= 15u;
			}
			GenSvm( &Svm );
		break;
		case 1u:
			if ( 10u == ( LL_TIMERS_TIMEDIFF( LL_ISR_SysTicks, CurrTicks ) ) )
			{
				CurrTicks = LL_ISR_SysTicks;
#if 0
				if (rampPresc > 20)
				{
					DoRamps(&SpeedRamp);
					DoRamps(&AmplRamp);
					rampPresc = 0u;
				}
				else
				{
					rampPresc++;
				}
				if (RATIO_SECTOR_TO_RAW <= CalcAngle)
				{
					if (1 == APP_HAL_SENS_IsHallTransOccurred())
					{
						// printf("CalcAngle initialized from soft limit\n");
						CalcAngle = 0;
						// printf("Svm.Angle:%u Svm.Amp:%u\n", Svm.Angle, Svm.Amp);
					}
				}
				if ((RATIO_SECTOR_TO_RAW + 5000) <= CalcAngle)
				{
					if (1 == APP_HAL_SENS_IsHallTransOccurred())
					{
						// printf("CalcAngle initialized from hard limit\n");
						CalcAngle = 0;
						// printf("Svm.Angle:%u Svm.Amp:%u\n", Svm.Angle, Svm.Amp);
					}
				}
				else
				{
					CalcAngle = (CalcAngle + 150/* + SpeedRamp.CurrVal*/);
					//Svm.Angle = ((RATIO_SECTOR_TO_RAW * APP_HALL_SENS_GetSector()) + CalcAngle);
				}
#endif
				Svm.Angle += 50;
				Svm.Amp = 12/*AmplRamp.CurrVal*/;

			    	//printf("Svm.Angle:%u Svm.Amp:%u\n", Svm.Angle, Svm.Amp);
				GenSvm( &Svm );
			}
		break;
		default:
		break;
	}
}










