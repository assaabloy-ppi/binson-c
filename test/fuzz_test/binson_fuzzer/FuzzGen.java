import java.nio.charset.Charset;
import java.util.Random;

import org.binson.Binson;
import org.binson.BinsonArray;

import org.apache.commons.lang3.RandomStringUtils;

public class FuzzGen {
	
	private static final int DepthLimit = 8; 
	private int fuzz;
	private Binson root;
	private Random  rn;
	
	/*
	 * 
	 */
	FuzzGen( int fuzz_factor )
	{
		rn = new Random();
		fuzz = fuzz_factor;
		root = new Binson();
	}

	/*
	 * 
	 */	
	String genStr()
	{
		//return RandomStringUtils.randomAscii(fuzz);
		return RandomStringUtils.random(fuzz);
	}
	
	private void GenArrayTree (BinsonArray node, int depth)
	{
		if (depth > DepthLimit) 
			depth = DepthLimit;
		
		for (int i=0; i<fuzz; i++)
		{
			switch( rn.nextInt(Math.min(fuzz, 7)) )
			{
				case 0:
					if (depth <= 0 ) continue;
					Binson obj2 = new Binson();
					GenObjectTree(obj2, depth-1);
					node.add(obj2);
				break;	
	
				case 1:
					if (depth <= 0 ) continue;
					BinsonArray arr2 = new BinsonArray();			
					GenArrayTree(arr2, depth-1);
					node.add(arr2);				
				break;	
				
				case 2: // BOOLEAN					
					node.add(rn.nextBoolean());
				break;				
					
				case 3: // 64-bit INTEGER
					node.add(rn.nextLong());
				break;				
			
				case 4: // DOUBLE
					node.add(rn.nextDouble());
				break;				
				
				case 5: // BYTES
					byte[] bbb = new byte[fuzz];
					rn.nextBytes(bbb);  // randomize byte array
					node.add(bbb);
				break;				

				case 6: // STRING
					node.add( genStr() );
				break;	
				
				default:
				continue;				
			}
		}
		
	}	
	
	private void GenObjectTree (Binson node, int depth)
	{
		if (depth > DepthLimit) 
			depth = DepthLimit;
		
		for (int i=0; i<fuzz; i++)
		{
			switch( rn.nextInt(Math.min(fuzz, 7)) )
			{
				case 0:
					if (depth <= 0 ) continue;
					Binson obj2 = new Binson();
					GenObjectTree(obj2, depth-1);
					node.put(genStr(), obj2);
				break;	
	
				case 1:
					if (depth <= 0 ) continue;
					BinsonArray arr2 = new BinsonArray();			
					GenArrayTree(arr2, depth-1);
					node.put(genStr(), arr2);				
				break;	
				
				
				case 2: // BOOLEAN					
					node.put(genStr(), rn.nextBoolean());
				break;				
					
				case 3: // 64-bit INTEGER
					node.put(genStr(), rn.nextLong());
				break;				
			
				case 4: // DOUBLE
					node.put(genStr(), rn.nextDouble());
				break;				
				
				case 5: // BYTES
					byte[] bbb = new byte[fuzz];
					rn.nextBytes(bbb);  // randomize byte array
					node.put(genStr(), bbb);
				break;				

				case 6: // STRING
					node.put(genStr(),  genStr() );
				break;	
				
				default:
				continue;							
			}
		}
		
	}
	
	/*
	 * 
	 */	
	Binson getRoot()
	{
		root = new Binson();
		
		GenObjectTree(root, fuzz);
		return root;
	}
	
}