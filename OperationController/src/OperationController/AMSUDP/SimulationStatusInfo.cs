using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace OperationController.AMSUDP
{
    public enum SimulationStatusInfo
    {
        IDLE = 1, DETECTEING = 2, CHASING = 3, SUCCESS = 4, FAIL = 5
    }
    public static class SimulationStatusInfoExtensions
    {
        public static int ToInt(this SimulationStatusInfo status)
        {
            return (int)status;
        }

        public static SimulationStatusInfo ValueOf(int value)
        {
            if (Enum.IsDefined(typeof(SimulationStatusInfo), value))
            {
                return (SimulationStatusInfo)value;
            }
            else
            {
                throw new ArgumentException("Invalid value for SimulationStatusInfo.");
            }
        }
    }

}
