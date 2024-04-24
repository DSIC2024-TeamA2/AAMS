using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace OperationController.AMSUDP
{
    public class ScenarioInfo
    {
        public long StartTime { get; set; }
        public Position AirThreatStartPosition { get; set; }
        public Position AirThreatEndPosition { get; set; }
        public float AirThreatSpeed { get; set; }
        public Position AntiAirMissilePosition { get; set; }
        public float AntiAirMissileSpeed { get; set; }

        public override string ToString()
        {
            return $"Scenario Start Time: {StartTime}, " +
                   $"Air Threat Start Position: {AirThreatStartPosition}, " +
                   $"Air Threat End Position: {AirThreatEndPosition}, " +
                   $"Air Threat Speed: {AirThreatSpeed}, " +
                   $"Anti-Air Missile Position: {AntiAirMissilePosition}, " +
                   $"Anti-Air Missile Speed: {AntiAirMissileSpeed}";
        }
    }
}
