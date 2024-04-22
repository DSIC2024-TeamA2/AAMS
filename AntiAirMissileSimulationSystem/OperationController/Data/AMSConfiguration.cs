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
        private static readonly string confFilePath = "conf";
        private long updateDuration;
        private int port;
        private static readonly AMSConfiguration instance = new();

        public AMSConfiguration()
        {
            Init();
        }

        private void Init()
        {
            try
            {
                using (StreamReader reader = new StreamReader(confFilePath))
                {
                    string line;
                    line = reader.ReadLine().Trim();
                    updateDuration = long.Parse(line);
                    line = reader.ReadLine().Trim();
                    port = int.Parse(line);
                }
            }
            catch (IOException e)
            {
                Console.WriteLine("The file could not be read:");
                Console.WriteLine(e.Message);
            }
        }

        public static AMSConfiguration GetInstance()
        {
            return instance;
        }

        public long UpdateDuration() { return updateDuration; }
        public int Port() { return port; }
    }
}
