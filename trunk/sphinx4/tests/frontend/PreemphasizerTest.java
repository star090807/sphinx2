/**
 * [[[copyright]]]
 */

package tests.frontend;

import edu.cmu.sphinx.frontend.CepstralMeanNormalizer;
import edu.cmu.sphinx.frontend.Cepstrum;
import edu.cmu.sphinx.frontend.CepstrumFrame;
import edu.cmu.sphinx.frontend.Data;
import edu.cmu.sphinx.frontend.Feature;
import edu.cmu.sphinx.frontend.FeatureExtractor;
import edu.cmu.sphinx.frontend.FeatureFrame;
import edu.cmu.sphinx.frontend.FrontEnd;
import edu.cmu.sphinx.frontend.Preemphasizer;
import edu.cmu.sphinx.frontend.SegmentEndPointSignal;
import edu.cmu.sphinx.frontend.Signal;
import edu.cmu.sphinx.frontend.Util;

import java.io.BufferedReader;
import java.io.FileInputStream;
import java.io.IOException;
import java.io.InputStream;
import java.util.StringTokenizer;


/**
 * Test program for the Preemphasizer.
 */
public class PreemphasizerTest {

    public static void main(String[] argv) {
	if (argv.length < 1) {
	    System.out.println("Usage: java FrontEnd <filename>");
	}

	Preemphasizer preemphasizer = new Preemphasizer();
	preemphasizer.setDump(true);

	FrontEnd frontend = new FrontEnd();

	frontend.addProcessor(preemphasizer);
	frontend.linkProcessors();

	try {
	    frontend.setInputStream(new FileInputStream(argv[0]));
	    frontend.run();
	} catch (Exception e) {
	    e.printStackTrace();
	}
    }
}
