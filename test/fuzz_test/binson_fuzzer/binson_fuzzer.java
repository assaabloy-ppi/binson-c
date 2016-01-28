import java.security.CodeSource;
import java.util.*;
import java.io.*;
import static java.nio.file.StandardOpenOption.*;

import java.net.URISyntaxException;
import java.nio.file.*;

import org.binson.Binson;
import org.binson.BinsonArray;



public class binson_fuzzer {
	
	static boolean toJSON = false; 
	
	static private int Generate(String filepath, int CurFactor, int CurIter)
	{
		Path p = Paths.get(filepath);
		byte[] bytes;
		
		FuzzGen fgen = new FuzzGen( CurFactor );
		if (toJSON)
		  bytes = fgen.getRoot().toJson().getBytes();
		else
		  bytes = fgen.getRoot().toBytes();
				
		try (OutputStream out = new BufferedOutputStream(
				Files.newOutputStream(p, CREATE, TRUNCATE_EXISTING))) {
				      out.write(bytes, 0, bytes.length);
		} catch (IOException x) {
			System.err.println(x);
		}
		   			
		return bytes.length;
		   
	}
	
	static private int run_script(String cmdline)
	{
		try
        {   
            Runtime rt = Runtime.getRuntime();
            Process proc = rt.exec(cmdline);
            InputStream stderr = proc.getInputStream();
            InputStreamReader isr = new InputStreamReader(stderr);
            BufferedReader br = new BufferedReader(isr);
            String line = null;
            //System.out.println("<ERROR>");
            //while ( (line = br.readLine()) != null)
            //    System.out.println(line);
            //System.out.println("</ERROR>");
            int exitVal = proc.waitFor();
            //System.out.println("Process exitValue: " + exitVal);
            return exitVal;
        } catch (Throwable t)
          {
            t.printStackTrace();
            return 255;
          }		
	}	
	
	/**
	 * @param args
	 * @throws URISyntaxException 
	 */
	public static void main(String[] args) throws URISyntaxException {
					
		System.out.println("Binson fuzzing data generator. v0.2");

		int[] arg = new int[3];
		if (args.length > 0) {
			for (int i=0; i<3; i++)
			{
			    try {
			        arg[i] = Integer.parseInt(args[i]);
			    } catch (NumberFormatException e) {
			        System.err.println("Argument" + args[0] + " must be an integer.");
			        System.exit(1);
			    }
			}
			
			if (args.length > 3 && args[3].equalsIgnoreCase("JSON"))
		        toJSON = true;
			
		}
		else
		{	
			System.out.println("\nUsage: binson_fuzzer.jar <f> <l> <x> [JSON]\n");		
			System.out.println("\t<f>: Integer in range 0..100 to specify starting fuzz factor");
			System.out.println("\t<l>: Integer in range 0..100 to specify ending fuzz factor");
			System.out.println("\t<x>: Integer to specify number of fuzz iterations before fuzz factor increment");
			System.out.println("\t[JSON]: Generate JSON output instead of Binson");			
	        System.exit(1);			
		}

		//String path = ClassLoader.getSystemClassLoader().getResource(".").getPath(); 
		//String path = "./";
		
		File fself = new File(binson_fuzzer.class.getProtectionDomain().getCodeSource().getLocation().getPath());
		String path = fself.getAbsoluteFile().getParentFile().getAbsolutePath()+"/";
		
		//String path = binson_fuzzer.class.getProtectionDomain().getCodeSource().getLocation()..getPath() + " ";
		String OutPath = path + "out/";
		
		File OutDir = new File(OutPath);
		
		try{
			OutDir.mkdir();
	    } 
	    catch(SecurityException se){
	    }     
		
		
		System.out.println("\nstarting fuzz factor: " + arg[0]);
		System.out.println("ending fuzz factor: " + arg[1]);
		System.out.println("iterations per factor: " + arg[2]);
		System.out.println();
		System.out.println("Working directory: " + path );
		System.out.println("External iteration cmd template: binson_fuzzer.sh " +  OutPath + "[factor]_[iter].bson");
	
		int CurFFactor = arg[0];
		int CutFIter = 0;
		int  fails = 0;
		long total = 0;
		
		while (CurFFactor <= arg[1] && CutFIter < arg[2])
		{					
			String ShortDataFilePath = Integer.toString(CurFFactor) + "_" + Integer.toString(CutFIter) + ".bson";
			String DataFilePath = OutPath + ShortDataFilePath;
			String ScriptCmdLine = path + "binson_fuzzer.sh " + DataFilePath;
			
			Generate( DataFilePath, CurFFactor, CutFIter);
			
			//System.out.println(ScriptCmdLine);
			int ret = run_script(ScriptCmdLine);
			
			if (ret != 0)
			{
				System.out.println("exited with error code " + ret + " ( " + ShortDataFilePath + " )");
				fails++;
			}
			
			System.out.print("\rProgress: " + total + ", failed:" + fails + "  ");
			
			
			total++;
			CutFIter++;
			if (CutFIter == arg[2])
			{
				CurFFactor++;
				CutFIter = 0;
			}
		}
		System.out.println("\n");
		
	}

}
