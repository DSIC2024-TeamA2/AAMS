using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace OperationController.AMSUDP
{
    public class AntiAirMissileInfo
    {
        public long CurrentTime { get; set; }
        public Position CurrentPosition { get; set; }
        public float CurrentSpeed { get; set; }
        public float CurrentAngle { get; set; }

        public override string ToString()
        {
            return $"AntiAirMissileInfo >>  Time:   {CurrentTime},  {CurrentPosition},  Speed:  {CurrentSpeed}Km/h, Angle:  {CurrentAngle:F1}°";
        }
    }
}
