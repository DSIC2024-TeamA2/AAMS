using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace OperationController.AMSUDP
{
    public class Position
    {
        public double Latitude { get; set; }
        public double Longitude { get; set; }
        public override string ToString()
        {
            return $"Latitude: {Latitude}, Longitude: {Longitude}";
        }
    }
}
