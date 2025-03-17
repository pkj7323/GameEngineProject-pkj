using PrimalEditor.Components;
using PrimalEditor.EngineAPIStructs;
using PrimalEditor.GameProject;
using PrimalEditor.Utilities;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Numerics;
using System.Runtime.InteropServices;
using System.Text;
using System.Threading.Tasks;

namespace PrimalEditor.EngineAPIStructs
{
    [StructLayout(LayoutKind.Sequential)]
    class TransformComponent
    {
        public Vector3 Position;
        public Vector3 Rotation;
        public Vector3 Scale = new Vector3(1, 1, 1);
    }
    [StructLayout(LayoutKind.Sequential)]
    class ScriptComponent
    {
        public IntPtr ScriptCreator;
    }
    [StructLayout(LayoutKind.Sequential)]
    class GameEntityDescriptor
    {
        public TransformComponent Transform = new TransformComponent();
        public ScriptComponent Script = new ScriptComponent();
    }
}

namespace PrimalEditor.DllWrappers
{
    static class EngineAPI
    {
        private const string _engineAPI = "EngineDll.dll";
        [DllImport(_engineAPI, CharSet=CharSet.Ansi)]
        public static extern int LoadGameCodeDll(string dllPath); //잘못되면 파라미터 이름탓
        [DllImport(_engineAPI)]
        public static extern int UnloadGameCodeDll();
        [DllImport(_engineAPI)]
        public static extern IntPtr GetScriptCreator(string name);
        [DllImport(_engineAPI)]
        [return : MarshalAs(UnmanagedType.LPStr)]
        public static extern string[] GetScriptNames();
        internal static class EntityAPI
        {
            [DllImport(_engineAPI)]
            private static extern int CreateGameEntity(GameEntityDescriptor descriptor);
            public static int CreateGameEntity(GameEntity entity)
            {
                GameEntityDescriptor descriptor = new GameEntityDescriptor();

                {
                    var c = entity.GetComponent<Transform>();
                    descriptor.Transform.Position = c.Position;
                    descriptor.Transform.Rotation = c.Rotation;
                    descriptor.Transform.Scale = c.Scale;
                }

                // script component
                {
                    // NOTE: here we also check if current project is not null, so we can tell whether the game code DLL
                    //       has been loaded or not. This way, creation of entities with a script component is deferred
                    //       until the DLL has been loaded.
                    var c = entity.GetComponent<Script>();
                    if (c != null && Project.Current != null)
                    {
                        if (Project.Current.AvailableScripts.Contains(c.Name))
                        {
                            descriptor.Script.ScriptCreator = GetScriptCreator(c.Name);
                        }
                        else
                        {
                            Logger.Log(MessageType.Error, $"Unable to find script with name {c.Name}. Game entity will be created without script component!");
                        }
                    }
                }
                return CreateGameEntity(descriptor);
            }
            [DllImport(_engineAPI)]
            private static extern int RemoveGameEntity(int entityId);
            public static int RemoveGameEntity(GameEntity entity)
            {
                return RemoveGameEntity(entity.EntityId);
            }
        }
    }
}
