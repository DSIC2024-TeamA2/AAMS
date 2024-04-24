using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace OperationController.Data
{
    public class AMSConfiguration
    {
        private static readonly string CONF_FILE_PATH = "../../conf";
        private static AMSConfiguration instance = new AMSConfiguration();
        public static AMSConfiguration GetInstance() { return instance; }
        public int UpdateDuration { get; set; }
        public int DetectionRadius { get; set; }
        private AMSConfiguration() 
        {
            Init();
        }

        private void Init()
        {
            using (StreamReader sr = new StreamReader(CONF_FILE_PATH))
            {
                UpdateDuration = int.Parse(sr.ReadLine());
                DetectionRadius = int.Parse(sr.ReadLine());
            }
        }
    }
}
