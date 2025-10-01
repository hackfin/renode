//
// (c) 2015, section5.se
//

using System;
using System.Threading;
using System.Runtime.InteropServices;
using System.Collections.Concurrent;
using Antmicro.Renode.Debugging;
using Antmicro.Renode.Logging;
using Antmicro.Renode.Exceptions;
using Antmicro.Renode.Utilities.Binding;
using Antmicro.Renode.Plugins.CoSimulationPlugin.Connection.Protocols;

namespace Antmicro.Renode.Plugins.CoSimulationPlugin.Connection
{
    public class NetppLocalConnection: LibraryConnection
    {
// #pragma warning disable 649
//         [Import(UseExceptionWrapper = false)]
//         private Action<IntPtr> getProperty;
//         [Import(UseExceptionWrapper = false)]
//         private Action<IntPtr> setProperty;
// #pragma warning restore 649

        public NetppLocalConnection(IEmulationElement parentElement,
			int timeout, Action<ProtocolMessage> receiveAction) : base(parentElement, timeout, receiveAction)
        {
        }

        public void SetProperty(string property_name, int value)
		{
			// setProperty(name, value);
		}

	}

}

