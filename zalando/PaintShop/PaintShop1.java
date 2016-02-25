/*
	For simplicity
		* I assume that input format is always correct.
		* I strongly avoid any over-engineering!
		* I did not tune HashMap bucket
		* tested only on Windows 7.1 x64 and Java x64 jdk1.8.0_45

	Please email knapiontek@gmail.com for clarification!

	run:
		javac PaintShop1.java
		java PaintShop1 <sample.txt>
*/

import java.io.*;
import java.util.*;

public class PaintShop1
{
	// helps to find out whether matte has to be used when glossy is fully ruled out
	static class CustomerInfo
	{
		public int matte = 0; // index of unique matte
		public int glossyInt = 0; // number of glossy colors for a single user
	};

	static class CustomerInfoList extends ArrayList<CustomerInfo>
	{
		// empty
	};

	public static void main(String[] args)
	{
		if (0 == args.length)
		{
			System.err.println("File expected.\nUsage:\n\tjava PaintShop1 <sample.txt>");
			System.exit(13);
		}

		Scanner scanner = null;
		try
		{
			String fileName = args[0];
			scanner = new Scanner(new File(fileName));

			run(scanner);
		}
		catch (Exception e)
		{
			System.err.println("Exception: " + e.getMessage());
			e.printStackTrace();
		}
		finally
		{
			if (scanner != null)
			{
				scanner.close();
			}
		}
	}

	static void run(Scanner scanner)
	{
		// number of test cases
		int C = scanner.nextInt();
		for (int c = 1; c <= C; c++)
		{
			// number of colors
			int N = scanner.nextInt();

			List<Integer> matteOnlyList = new ArrayList<Integer>();

			// search-index which is used later to remove impossible glossy
			Map<Integer, CustomerInfoList> glossyCustomerInfoMap = new HashMap<Integer, CustomerInfoList>();

			// number of customers
			int M = scanner.nextInt();
			for (int m = 1; m <= M; m++)
			{
				CustomerInfo customerInfo = new CustomerInfo();

				// number of color types
				int T = scanner.nextInt();
				for (int t = 1; t <= T; t++)
				{
					int X = scanner.nextInt(); // color index
					int Y = scanner.nextInt(); // glossy or matte

					if (Y == 0)
					{
						customerInfo.glossyInt++;

						// building search-index
						CustomerInfoList customerInfoList = glossyCustomerInfoMap.get(X);
						if (customerInfoList == null)
						{
							customerInfoList = new CustomerInfoList();
							glossyCustomerInfoMap.put(X, customerInfoList);
						}
						customerInfoList.add(customerInfo);
					}
					else if (T == 1)
					{
						matteOnlyList.add(X);
					}
					else
					{
						assert(customerInfo.matte == 0);
						customerInfo.matte = X;
					}
				}
			}

			// print to standard output
			System.out.print("Case #" + c + ":");
			if (analyze(matteOnlyList, glossyCustomerInfoMap))
			{
				Set<Integer> matteOnlySet = new HashSet<Integer>(matteOnlyList);
				for (int n = 1; n <= N; n++)
				{
					System.out.print(matteOnlySet.contains(n) ? " 1" : " 0");
				}
			}
			else
			{
				System.out.print(" IMPOSSIBLE");
			}
			System.out.println();
		}
	}

	static boolean analyze(List<Integer> matteOnlyList, Map<Integer, CustomerInfoList> glossyCustomerInfoMap)
	{
		for (int i = 0; i < matteOnlyList.size(); i++) // matteOnlyList modified in the inner loop
		{
			int matteColor = matteOnlyList.get(i);

			// remove glossy, we know it has to be matte
			CustomerInfoList customerInfoList = glossyCustomerInfoMap.remove(matteColor);
			if (customerInfoList != null)
			{
				for (CustomerInfo customerInfo : customerInfoList)
				{
					customerInfo.glossyInt--;
					if (customerInfo.glossyInt == 0)
					{
						if (customerInfo.matte > 0) // new matte comes up, re-analyze it as well
						{
							matteOnlyList.add(customerInfo.matte);
						}
						else
						{
							return false; // impossible
						}
					}
				}
			}
		}
		return true;
	}
}
